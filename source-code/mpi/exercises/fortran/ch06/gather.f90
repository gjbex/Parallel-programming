program gather
    use, intrinsic :: iso_fortran_env, only: output_unit, error_unit
    use :: mpi_f08
    implicit none
    integer, parameter :: root = 0, TAG = 17
    integer :: my_rank, nr_procs, i, ierr
    integer :: send_buffer
    integer, dimension(:), allocatable :: recv_buffer

    call MPI_Init()
    call MPI_Comm_rank(MPI_COMM_WORLD, my_rank)
    call MPI_Comm_size(MPI_COMM_WORLD, nr_procs)
    if (my_rank == root) then
        allocate(recv_buffer(nr_procs), stat=ierr)
        if (ierr /= 0) then
            write (unit=error_unit, fmt='(A, I0)') 'Error allocating memory: ', ierr
            call MPI_Abort(MPI_COMM_WORLD, 1)
        end if
    end if
    send_buffer = my_rank
    call MPI_Gather(send_buffer, 1, MPI_INTEGER, recv_buffer, 1, MPI_INTEGER, root, MPI_COMM_WORLD)
    if (my_rank == root) then
        write (output_unit, fmt='(I0)') recv_buffer
        deallocate(recv_buffer)
    end if
    call MPI_Finalize()
end program gather
