program ring_nonblocking
    use :: mpi_f08
    implicit none
    integer, parameter :: TAG = 17
    integer :: my_rank, nr_procs, source, dest, recv_buffer, sum, i
    integer, asynchronous :: send_buffer
    type(MPI_Request) :: request

    call MPI_Init()
    call MPI_Comm_rank(MPI_COMM_WORLD, my_rank)
    call MPI_Comm_size(MPI_COMM_WORLD, nr_procs)
    send_buffer = my_rank
    sum = 0
    source = mod(my_rank - 1 + nr_procs, nr_procs)
    dest = mod(my_rank + 1, nr_procs)
    do i = 1, nr_procs
        call MPI_Issend(send_buffer, 1, MPI_INTEGER, dest, TAG, MPI_COMM_WORLD, request)
        call MPI_Recv(recv_buffer, 1, MPI_INTEGER, source, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE)
        sum = sum + recv_buffer
        call MPI_WAIT(request, MPI_STATUS_IGNORE)
        if (.not. MPI_ASYNC_PROTECTS_NONBLOCKING) &
            call MPI_F_sync_reg(send_buffer)
        send_buffer = recv_buffer
    end do
    print '(2(A, I0))', 'rank ', my_rank, ': ', sum
    call MPI_Finalize()
end program ring_nonblocking
