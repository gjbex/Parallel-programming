program ping_pong_benchmark
    use, intrinsic :: iso_fortran_env, only : error_unit, DP => REAL64
    use :: mpi_f08
    implicit none
    integer, parameter :: TAG = 17, root = 0
    integer :: my_rank, nr_procs, nr_ping_pongs = 10, message_size = 1, ierr, i
    integer :: source, dest
    character(len=32) :: str_buffer
    character, dimension(:), allocatable :: buffer
    real(kind=DP) :: start_time, end_time

    call MPI_Init()
    call MPI_Comm_rank(MPI_COMM_WORLD, my_rank)
    call MPI_Comm_size(MPI_COMM_WORLD, nr_procs)
    if (my_rank == root) then
        if (nr_procs < 2) then
            write (error_unit, '(A)') 'run with at least two processes'
            call MPI_Abort(MPI_COMM_WORLD, 1)
        end if
        if (command_argument_count() > 0) then
            call get_command_argument(1, str_buffer)
            read (str_buffer, *) nr_ping_pongs
        end if
        if (command_argument_count() > 1) then
            call get_command_argument(2, str_buffer)
            read (str_buffer, *) message_size
        end if
    end if
    call MPI_Bcast(nr_ping_pongs, 1, MPI_INTEGER, root, MPI_COMM_WORLD)
    call MPI_Bcast(message_size, 1, MPI_INTEGER, root, MPI_COMM_WORLD)
    ! create buffer
    allocate(buffer(message_size), stat=ierr)
    if (ierr /= 0) then
        write (error_unit, '(A, I0)') 'can not allocate buffer for process ', my_rank
        call MPI_Abort(MPI_COMM_WORLD, 2)
    end if
    do i = 1, message_size
        buffer(i) = char(iachar('0') + mod(i, 10))
    end do
    if (my_rank == 0) then
        source = 1
        dest = 1
    else if (my_rank == 1) then
        source = 0
        dest = 0
    end if
    ! warm-up
    if (my_rank == 0) then
        call MPI_Send(buffer, message_size, MPI_CHARACTER, dest, TAG, MPI_COMM_WORLD)
        call MPI_Recv(buffer, message_size, MPI_CHARACTER, source, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE)
    else if (my_rank == 1) then
        call MPI_Recv(buffer, message_size, MPI_CHARACTER, source, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE)
        call MPI_Send(buffer, message_size, MPI_CHARACTER, dest, TAG, MPI_COMM_WORLD)
    end if
    ! start ping-pong
    start_time = MPI_Wtime()
    do i = 1, nr_ping_pongs
        if (my_rank == 0) then
            call MPI_Send(buffer, message_size, MPI_CHARACTER, dest, TAG, MPI_COMM_WORLD)
            call MPI_Recv(buffer, message_size, MPI_CHARACTER, source, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE)
        else if (my_rank == 1) then
            call MPI_Recv(buffer, message_size, MPI_CHARACTER, source, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE)
            call MPI_Send(buffer, message_size, MPI_CHARACTER, dest, TAG, MPI_COMM_WORLD)
        end if
    end do
    end_time = MPI_Wtime()
    if (my_rank == root) then
        print '(A, I0, A, F12.6)', 'time to send a message of size ', message_size, ': ', &
            0.5*(end_time - start_time)/nr_ping_pongs
    end if
    call MPI_Finalize()
end program ping_pong_benchmark
