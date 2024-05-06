program ring_deadlock
    use :: mpi_f08
    implicit none
    integer, parameter :: TAG = 17
    integer :: my_rank, nr_procs, source, dest, buffer, sum, i

    call MPI_Init()
    call MPI_Comm_rank(MPI_COMM_WORLD, my_rank)
    call MPI_Comm_size(MPI_COMM_WORLD, nr_procs)
    buffer = my_rank
    sum = 0
    source = mod(my_rank - 1 + nr_procs, nr_procs)
    dest = mod(my_rank + 1, nr_procs)
    do i = 1, nr_procs
        call MPI_Ssend(buffer, 1, MPI_INTEGER, dest, TAG, MPI_COMM_WORLD)
        call MPI_Recv(buffer, 1, MPI_INTEGER, source, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE)
        sum = sum + buffer
    end do
    print '(2(A, I0))', 'rank ', my_rank, ': ', sum
    call MPI_Finalize()
end program ring_deadlock
