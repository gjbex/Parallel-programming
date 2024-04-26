program hello
    use :: mpi_f08
    implicit none

    call MPI_Init()
    print '(A)', "Hello, world!"
    call MPI_Finalize()
end program hello
