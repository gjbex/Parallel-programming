#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_section(MPI_Comm comm, int root, const char *heading,
                          const char *text) {
    const int length_tag = 0;
    const int text_tag = 1;
    int comm_rank, comm_size, length;

    MPI_Comm_rank(comm, &comm_rank);
    MPI_Comm_size(comm, &comm_size);

    length = (int) strlen(text);
    if (comm_rank == root) {
        int i;

        if (heading != NULL) {
            printf("%s\n\n", heading);
        }
        printf("%s", text);
        for (i = 1; i < comm_size; i++) {
            MPI_Status status;
            int sender_length;
            char *buffer;

            MPI_Recv(&sender_length, 1, MPI_INT, i, length_tag,
                     comm, &status);
            buffer = malloc(((size_t) sender_length + 1)*sizeof(char));
            if (buffer == NULL) {
                fprintf(stderr, "failed to allocate output buffer\n");
                MPI_Abort(comm, 1);
            }
            MPI_Recv(buffer, sender_length + 1, MPI_CHAR, i, text_tag,
                     comm, MPI_STATUS_IGNORE);
            printf("%s", buffer);
            free(buffer);
        }
        printf("\n");
        fflush(stdout);
    } else {
        MPI_Send(&length, 1, MPI_INT, root, length_tag, comm);
        MPI_Send(text, length + 1, MPI_CHAR, root, text_tag, comm);
    }

    MPI_Barrier(comm);
}

int main() {
    int my_rank, my_size, sub_rank, sub_size, color, remote_leader,
        inter_rank, root;
    int ranges[1][3];
    MPI_Comm sub_comm, peer_comm, inter_comm;
    MPI_Group world_group, sub_group;
    int send_buffer, recv_buffer;
    char section[256];

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &my_size);

    /* create a group for MPI_COMM_WORLD */
    MPI_Comm_group(MPI_COMM_WORLD, &world_group);

    /* determinte color and ranges */
    color = my_rank < my_size/3 ? 0 : 1;
    ranges[0][0] = color*(my_size/3);  // start index
    ranges[0][1] = color == 0 ? my_size/3 - 1 : my_size - 1; // end index
    ranges[0][2] = 1; // index stride

    /* split the group into subgroups based on ranges,
     * the first subcommunicator (color == 0) and the second
     * (color == 1) */
    MPI_Group_range_incl(world_group, 1, ranges, &sub_group);
    MPI_Comm_create(MPI_COMM_WORLD, sub_group, &sub_comm);
    
    /* determine rank in subcommunicator */
    MPI_Comm_rank(sub_comm, &sub_rank);
    MPI_Comm_size(sub_comm, &sub_size);

    /* show ranks and ranges */
    snprintf(section, sizeof(section),
             "original rank %d -> subrank %d: range from %d to %d\n",
             my_rank, sub_rank, ranges[0][0], ranges[0][1]);
    print_section(MPI_COMM_WORLD, 0, "# create subcommunicators", section);

    /* set up intercommunicator and print ranks in it */
    MPI_Comm_dup(MPI_COMM_WORLD, &peer_comm);
    if (color == 0) {
        remote_leader = sub_size;
    } else {
        remote_leader = 0;
    }
    MPI_Intercomm_create(sub_comm, 0, peer_comm, remote_leader, 0,
                         &inter_comm);
    MPI_Comm_rank(inter_comm, &inter_rank);
    snprintf(section, sizeof(section),
             "rank %2d: remote leader %2d\n"
             "rank %2d -> intercommunicator rank %2d\n",
             my_rank, remote_leader, my_rank, inter_rank);
    print_section(MPI_COMM_WORLD, 0, "# set up intercommunicator", section);

    /* do a reduce in each subcommunicator, and send the result
     * to all members of the other subcommunicator */
    send_buffer = sub_rank;
    MPI_Allreduce(&send_buffer, &recv_buffer, 1, MPI_INT,
                  MPI_SUM, inter_comm);
    snprintf(section, sizeof(section), "rank %2d: allreduce %2d\n",
             my_rank, recv_buffer);
    print_section(MPI_COMM_WORLD, 0,
                  "# do all reduce on each subcommunicator and send to other subcommunicator",
                  section);

    /* do a reduce on the first subcommunicator (color == 0) and
     * send it to the leader of the second subcommunicator
     * (color == 1) */
    if (color == 0) {
        root = 0;
    } else if (sub_rank == 0) {
        root = MPI_ROOT;
    } else {
        root = MPI_PROC_NULL;
    }
    recv_buffer = -1;
    MPI_Reduce(&sub_rank, &recv_buffer, 1, MPI_INT, MPI_SUM, root,
               inter_comm);
    snprintf(section, sizeof(section), "rank %2d: reduce %d\n",
             my_rank, recv_buffer);
    print_section(MPI_COMM_WORLD, 0,
                  "# do reduce on first subcommunicator and send to leader of other subcommunicator",
                  section);

    /* do a reduce on the second subcommunicator (color == 1) and
     * send it to the leader of the first subcommunicator
     * (color == 0) */
    if (color == 1) {
        root = 0;
    } else if (sub_rank == 0) {
        root = MPI_ROOT;
    } else {
        root = MPI_PROC_NULL;
    }
    recv_buffer = -1;
    MPI_Reduce(&sub_rank, &recv_buffer, 1, MPI_INT, MPI_SUM, root,
               inter_comm);
    snprintf(section, sizeof(section), "rank %2d: reduce %d\n",
             my_rank, recv_buffer);
    print_section(MPI_COMM_WORLD, 0,
                  "# do reduce on second subcommunicator and send to leader of first subcommunicator",
                  section);
    /* do a broadcast from rank 0 in the second subcommunicator
     * (color == 1) to all processes of the first
     * subcommunicator (color == 0), don't send anything to the
     * other members of the second subcommunicator (color == 1) */
    if (color == 0) {
        root = 0;
        send_buffer = -1;
    } else if (sub_rank == 0) {
        root = MPI_ROOT;
        send_buffer = 13;
    } else {
        root = MPI_PROC_NULL;
        send_buffer = -1;
    }
    MPI_Bcast(&send_buffer, 1, MPI_INT, root, inter_comm);
    snprintf(section, sizeof(section), "rank %2d: bcast %d\n",
             my_rank, send_buffer);
    print_section(MPI_COMM_WORLD, 0,
                  "# do bcast from 0 to other subcommunicator", section);

    /* do a broadcast from rank 0 in the first subcommunicator
     * (color == 0) to all processes of the second
     * subcommunicator (color == 1), don't send anything to the
     * other members of the second subcommunicator (color == 1) */
    if (color == 1) {
        root = 0;
        send_buffer = -1;
    } else if (sub_rank == 0) {
        root = MPI_ROOT;
        send_buffer = 17;
    } else {
        root = MPI_PROC_NULL;
        send_buffer = -1;
    }
    MPI_Bcast(&send_buffer, 1, MPI_INT, root, inter_comm);
    snprintf(section, sizeof(section), "rank %2d: bcast %d\n",
             my_rank, send_buffer);
    print_section(MPI_COMM_WORLD, 0,
                  "# do bcast from 0 in second subcommunitator to first subcommunicator", section);
    MPI_Finalize();
    return 0;
}
