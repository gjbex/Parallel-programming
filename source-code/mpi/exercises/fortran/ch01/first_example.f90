program first_example
    use, intrinsic :: iso_fortran_env, only: FP => REAL64, input_unit, output_unit
    use :: mpi_f08
    implicit none
    ! MPI variables
    integer, parameter :: root = 0, tag = 17
    integer :: my_rank, nr_procs
    ! Application variables
    integer :: n
    real(kind=FP) :: result

    call MPI_Init()
    call MPI_Comm_rank(MPI_COMM_WORLD, my_rank)
    call MPI_Comm_size(MPI_COMM_WORLD, nr_procs)
    if (my_rank == root) then
        write (output_unit, '(A)', advance='no') 'Enter n: '
        flush(output_unit)
        read (input_unit, *) n
    end if
    call MPI_Bcast(n, 1, MPI_INTEGER, root, MPI_COMM_WORLD)
    result = real(my_rank*n, kind=FP)
    print '(4(A,I0),A,F0.0)', 'I am process ', my_rank, ' out of ', nr_procs, &
        ' handling the ', my_rank, 'th part of ', n, &
        ' result = ', result
    call MPI_Finalize()
end program first_example
