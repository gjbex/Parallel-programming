program gather_skeleton
    use :: mpi_f08
    implicit none
    integer, parameter :: root = 0, TAG = 17
    integer :: my_rank, nr_procs, i
    integer :: send_buffer, recv_buffer

    call MPI_Init()
    call MPI_Comm_rank(MPI_COMM_WORLD, my_rank)
    call MPI_Comm_size(MPI_COMM_WORLD, nr_procs)
    if (my_rank == root) then
        print '(I0)', my_rank
        do i = 1, nr_procs - 1
            call MPI_Recv(recv_buffer, 1, MPI_INTEGER, i, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE)
            print '(I0)', recv_buffer
        end do
        print *, ''
    else
        send_buffer = my_rank
        call MPI_Send(send_buffer, 1, MPI_INTEGER, root, TAG, MPI_COMM_WORLD)
    end if
    call MPI_Finalize()
end program gather_skeleton
