program julia_set
    use, intrinsic :: iso_fortran_env, only: sp => REAL32, output_unit, &
        error_unit
    implicit none

    integer, parameter :: max_iters = 255
    integer :: width, height
    complex(sp), dimension(:, :), allocatable :: c
    integer, dimension(:, :), allocatable :: iter
    integer :: i, j, k, status

    ! read command line arguments and set width and height
    call command_arguments(width, height)

    ! allocate and initialize z
    allocate (c(width, height), stat = status)
    if (status /= 0) then
        write (unit=error_unit, fmt='(a)') 'Failed to allocate z'
        stop 2
    end if
    call initiallize_constant(c)

    ! allocate and initialize iter
    allocate (iter(width, height), stat = status)
    if (status /= 0) then
        write (unit=error_unit, fmt='(a)') 'Failed to allocate iter'
        stop 2
    end if
    iter = 0

    ! iterate
    call iterate(c, iter)

    ! write the iterations to standard output
    do j = 1, height
        do i = 1, width
            write (unit=output_unit, fmt='(i0, a)', advance='no') iter(i, j), ' '
        end do
        write (unit=output_unit, fmt='(a)') ''
    end do

    ! deallocate z and iter
    deallocate(c, iter)

contains

    subroutine iterate(c, iters)
        implicit none
        complex(sp), dimension(:, :), intent(in) :: c
        integer, dimension(:, :), intent(inout) :: iters
        integer :: i, j, iteration, width, height
        complex(sp) :: z

        width = size(c, 1)
        height = size(c, 2)

        do j = 1, height
            do i = 1, width
                z = (0.0_sp, 0.0_sp)
                do iteration = 0, max_iters
                    z = z**2 + c(i, j)
                    if (abs(z) > 2.0_sp) then
                        iters(i, j) = iteration
                        exit
                    end if
                end do
            end do
        end do
    end subroutine iterate

    subroutine initiallize_constant(c)
        implicit none
        complex(sp), dimension(:, :), intent(out) :: c
        integer :: i, j, width, height

        width = size(c, 1)
        height = size(c, 2)

        do j = 1, height
            do i = 1, width
                c(i, j) = (i - 1)*(1.0_sp, 0.0_sp)/(width - 1) + &
                    (j - 1)*(0.0_sp, 1.0_sp)/(height - 1)
            end do
        end do
    end subroutine initiallize_constant

    subroutine command_arguments(width, height)
        implicit none
        integer, intent(out) :: width, height
        character(len=1024) :: arg
        integer :: status

        ! if no command line argumetns are given, use default values
        ! if only one command line argument is given, assume that width and
        ! height are equal
        ! if two command line arguments are given, use them as width and height
        if (command_argument_count() == 0) then
            width = 640
            height = 480
        else if (command_argument_count() == 1) then
            call get_command_argument(1, arg)
            read (arg, *, iostat=status) width
            if (status /= 0) then
                write(error_unit, fmt='(a)') 'Failed to read width'
                stop 1
            end if
            height = width
        else if (command_argument_count() == 2) then
            call get_command_argument(1, arg)
            read (arg, *, iostat=status) width
            if (status /= 0) then
                write (unit=error_unit, fmt='(a)') 'Failed to read width'
                stop 1
            end if
            call get_command_argument(2, arg)
            read (arg, *, iostat=status) height
            if (status /= 0) then
                write (unit=error_unit, fmt='(a)') 'Failed to read height'
                stop 1
            end if
        else if (command_argument_count() /= 2) then
            write (unit=error_unit, fmt='(a)') 'Usage: julia_set width height'
            stop 1
        end if
    end subroutine command_arguments

end program julia_set
