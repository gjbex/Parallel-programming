program sendrecv
    use, intrinsic :: iso_fortran_env, only : dp => REAL64
    use :: mpi
    implicit none
    integer :: size, rank, ierr, tag = 11, left, right
    integer, dimension(MPI_STATUS_SIZE) :: status
    real(kind=dp) :: send_msg, recv_msg

    call MPI_Init(ierr)
    call MPI_Comm_rank(MPI_COMM_WORLD, rank, ierr)
    call MPI_Comm_size(MPI_COMM_WORLD, size, ierr)
    call neighbours(rank, size, left, right)
    send_msg = rank*1.1_dp
    call MPI_Sendrecv(send_msg, 1, MPI_DOUBLE_PRECISION, left, tag, &
                      recv_msg, 1, MPI_DOUBLE_PRECISION, right, tag, &
                      MPI_COMM_WORLD, MPI_STATUS_IGNORE, ierr)
    print '(I0, A, F10.2, A, I0)', &
            rank, ' received ', recv_msg, ' from ', right
    call MPI_Sendrecv(send_msg, 1, MPI_DOUBLE_PRECISION, right, tag, &
                      recv_msg, 1, MPI_DOUBLE_PRECISION, left, tag, &
                      MPI_COMM_WORLD, MPI_STATUS_IGNORE, ierr)
    print '(I0, A, F10.2, A, I0)', &
            rank, ' received ', recv_msg, ' from ', left
    call MPI_Finalize(ierr)

contains

    subroutine neighbours(rank, size,  left, right)
        implicit none
        integer, intent(in) :: rank, size
        integer, intent(out) :: left, right
        if (rank == 0) then
            left = size - 1
        else
            left = rank - 1
        end if
        if (rank == size - 1) then
            right = 0
        else
            right = rank + 1
        end if
    end subroutine neighbours

end program sendrecv
