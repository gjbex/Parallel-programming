program first_dd_b
    use, intrinsic :: iso_fortran_env, only: error_unit
    use :: mpi_f08
    implicit none
    integer :: my_rank, nr_procs, lb, ub, data_size, n, nr_extras
    integer, parameter :: root = 0
    character(len=30) :: buffer

    call MPI_Init()
    call MPI_Comm_rank(MPI_COMM_WORLD, my_rank)
    call MPI_Comm_size(MPI_COMM_WORLD, nr_procs)
    if (my_rank == root) then
        if (command_argument_count() /= 1) then
            write(error_unit, '(A)') 'Usage: mpirun -np <nr_procs> ./first_example <n>'
            call MPI_Abort(MPI_COMM_WORLD, 1)
        else
            call get_command_argument(1, buffer)
            read(buffer, *) n
        end if
    end if
    call MPI_Bcast(n, 1, MPI_INTEGER, root, MPI_COMM_WORLD)
    data_size = n/nr_procs
    nr_extras = mod(n, nr_procs)
    if (my_rank < nr_extras) then
        data_size = data_size + 1
        lb = my_rank*data_size + 1
    else if (data_size > 0) then
        lb = my_rank*data_size + nr_extras + 1
    else
        data_size = 0
        lb = -1
    end if
    ub = lb + data_size - 1
    print '(A, I0, A, 3(A, I0))', 'Process ', my_rank, ': ', &
        'lb=', lb, ' ub=', ub, ' data_size=', data_size
    call MPI_Finalize()
end program first_dd_b
