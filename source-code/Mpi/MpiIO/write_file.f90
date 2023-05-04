program write_subarray_file
    use :: mpi_f08
    implicit none
    integer, parameter :: ndims = 2
    integer :: comm_rank, comm_size
    integer, dimension(ndims) :: cart_coords, cart_dims, global_dims
    logical, dimension(ndims) :: periodic
    type(MPI_Comm) :: cart_comm

    call MPI_Init()
    call MPI_Comm_rank(MPI_COMM_WORld, comm_rank)
    if (comm_rank == 0) then
        call get_arguments(global_dims)
    end if
    call MPI_Bcast(global_dims, 2, MPI_INTEGER, 0, MPI_COMM_WORLD)
    call MPI_Comm_size(MPI_COMM_WORld, comm_size)
    cart_dims = 0
    periodic = .false.
    call MPI_Dims_create(comm_size, ndims, cart_dims)
    call MPI_Cart_create(MPI_COMM_WORld, ndims, cart_dims, &
                         periodic, .false., cart_comm)
    call MPI_Comm_rank(MPI_COMM_WORld, comm_rank)
    call MPI_Cart_coords(cart_comm, comm_rank, ndims, cart_coords)
    print '(5I5)', comm_rank, cart_coords(1), cart_coords(2), &
        global_dims(1), global_dims(2)
    call MPI_Finalize()

contains

    subroutine get_arguments(dims)
        use, intrinsic :: iso_fortran_env, only : error_unit
        implicit none
        integer, dimension(ndims), intent(out) :: dims
        character(len=20) :: buffer, msg
        integer :: istat

        if (command_argument_count() /= 2) then
            write (unit=error_unit, fmt='(A)') &
                'error: expecting 2D array dimensions'
            stop 1
        end if
        call get_command_argument(1, buffer)
        read (buffer, fmt=*, iomsg=msg, iostat=istat) dims(1)
        if (istat /= 0) then
            write (unit=error_unit, fmt='(2A)') &
                'error: ', trim(msg)
            stop 2
        end if
        call get_command_argument(2, buffer)
        read (buffer, fmt=*, iomsg=msg, iostat=istat) dims(2)
        if (istat /= 0) then
            write (unit=error_unit, fmt='(2A)') &
                'error: ', trim(msg)
            stop 2
        end if
    end subroutine get_arguments

end program write_subarray_file
