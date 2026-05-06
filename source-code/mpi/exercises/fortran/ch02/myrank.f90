program myrank
    use :: mpi_f08
    implicit none
    integer :: my_rank, nr_procs

    call MPI_Init()
    call MPI_Comm_rank(MPI_COMM_WORLD, my_rank)
    call MPI_Comm_size(MPI_COMM_WORLD, nr_procs)
    if (my_rank == 0) then
        print '(A)', "Hello, world!"
    end if
    print '(A, I0, A, I0)', "I am process ", my_rank, " of ", nr_procs
    call MPI_Finalize()
end program myrank
