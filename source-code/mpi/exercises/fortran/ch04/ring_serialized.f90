program ring_serialized
    use :: mpi_f08
    implicit none
    integer, parameter :: TAG = 17
    integer :: my_rank, nr_procs, source, dest, send_buffer, recv_buffer, sum, i

    call MPI_Init()
    call MPI_Comm_rank(MPI_COMM_WORLD, my_rank)
    call MPI_Comm_size(MPI_COMM_WORLD, nr_procs)
    send_buffer = my_rank
    sum = 0
    source = mod(my_rank - 1 + nr_procs, nr_procs)
    dest = mod(my_rank + 1, nr_procs)
    do i = 1, nr_procs
        if (my_rank == 0) then
            call MPI_Ssend(send_buffer, 1, MPI_INTEGER, dest, TAG, MPI_COMM_WORLD)
            call MPI_Recv(recv_buffer, 1, MPI_INTEGER, source, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE)
        else
            call MPI_Recv(recv_buffer, 1, MPI_INTEGER, source, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE)
            call MPI_Ssend(send_buffer, 1, MPI_INTEGER, dest, TAG, MPI_COMM_WORLD)
        end if
        sum = sum + recv_buffer
        send_buffer = recv_buffer
    end do
    print '(2(A, I0))', 'rank ', my_rank, ': ', sum
    call MPI_Finalize()
end program ring_serialized
