program heat

    use, intrinsic :: iso_fortran_env, only : FP => REAL32, error_unit
    implicit none
    real(kind=FP), parameter :: diff_stop = 1e-5_FP
    real(kind=FP), dimension(:, :), allocatable, target :: temp_data, prev_temp_data
    real(kind=FP), dimension(:, :), pointer :: temp, prev_temp, tmp
    real(Kind=FP) :: diff, max_diff
    integer :: t, n = 10, t_max = 5, istat, i, j
    character(len=1024) :: buffer

    ! get command line arguments
    if (command_argument_count() > 0) then
        call get_command_argument(1, buffer)
        read (buffer, *) n
    end if

    if (command_argument_count() > 1) then
        call get_command_argument(2, buffer)
        read (buffer, *) t_max
    end if

    ! allocate matrices
    allocate(temp_data(n, n), stat=istat)
    if (istat /= 0) then
        write (unit=error_unit, fmt='(A)') 'can not allocate temp'
        stop 1
    end if
    temp => temp_data
    allocate(prev_temp_data(n, n), stat=istat)
    if (istat /= 0) then
        write (unit=error_unit, fmt='(A)') 'can not allocate prev_temp'
        stop 1
    end if
    prev_temp => prev_temp_data

    ! initialize temperatures
    do j = 1, n
        temp_data(1, j) = 1.0_FP
        prev_temp_data(1, j) = 1.0_FP
        do i = 2, n
            temp_data(i, j) = 0.0_FP
            prev_temp_data(i, j) = 0.0_FP
        end do
    end do 

    do t = 1, t_max
        max_diff = -1e10_FP
        do j = 2, n - 1
            do i = 2, n - 1
                temp(i, j) = 0.25_FP*(prev_temp(i - 1, j) + prev_temp(i + 1, j) + &
                    prev_temp(i, j - 1) + prev_temp(i, j + 1))
                diff = abs(temp(i, j) - prev_temp(i, j))
                if (max_diff < diff) then
                    max_diff = diff
                end if
            end do
        end do
        write (unit=error_unit, fmt='(A, I0, A, F12.6)') 'step ', t, ': ', max_diff
        if (max_diff < diff_stop) exit
        tmp => temp
        temp => prev_temp
        prev_temp => tmp
    end do

    call print_system(temp)

    ! deallocate matrices
    deallocate(temp_data)
    deallocate(prev_temp_data)

contains

    subroutine print_system(temp)
        implicit none
        real(kind=FP), dimension(:, :), intent(in) :: temp
        integer :: i

        do i = 1, size(temp, 1)
            print '(*(F12.6))', temp(i, :)
        end do
    end subroutine print_system

end program heat
