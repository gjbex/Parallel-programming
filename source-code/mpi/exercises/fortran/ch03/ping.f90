program ping
    use, intrinsic :: iso_fortran_env, only : error_unit
    use :: mpi_f08
    implicit none
    integer, parameter :: TAG = 17
    integer :: my_rank, nr_procs
    integer :: send_buffer, recv_buffer, dest, source
    type(MPI_Status) :: status

    call MPI_Init()
    call MPI_Comm_rank(MPI_COMM_WORLD, my_rank)
    call MPI_Comm_size(MPI_COMM_WORLD, nr_procs)
    if (nr_procs < 2 .and. my_rank == 0) then
        write (error_unit, '(A)') 'run with at least two processes'
        call MPI_Abort(MPI_COMM_WORLD, 1)
    end if
    if (my_rank == 0) then
        send_buffer = my_rank
        dest = 1
        print '(3(A, I0))', 'rank ', my_rank, ' sending ', send_buffer, ' to ', dest
        call MPI_Send(send_buffer, 1, MPI_INTEGER, dest, TAG, MPI_COMM_WORLD)
    else if (my_rank == 1) then
        source = 0
        call MPI_Recv(recv_buffer, 1, MPI_INTEGER, source, TAG, MPI_COMM_WORLD, status)
        print '(3(A, I0))', 'rank ', my_rank, ' received ', recv_buffer, ' from ', status%MPI_SOURCE
    end if
    call MPI_Finalize()
end program ping
