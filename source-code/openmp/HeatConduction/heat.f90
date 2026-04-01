program heat

    use, intrinsic :: iso_fortran_env, only : FP => REAL32, error_unit
    implicit none
    real(kind=FP), parameter :: diff_stop = 1e-3_FP
    real(kind=FP), dimension(:, :), allocatable, target :: temp_data, prev_temp_data
    real(kind=FP), dimension(:, :), pointer :: temp, prev_temp, tmp
    real(kind=FP) :: diff, max_diff, b_value
    integer :: t, n = 10, t_max = 5, istat, i, j, max_t
    character(len=1024) :: buffer
    logical :: is_done

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
    !$omp parallel default(none) shared(temp, prev_temp, n) private(i, j, b_value)
        !$omp do collapse(2)
        do j = 2, n - 1
            do i = 2, n
                temp(i, j) = 0.0_FP
                prev_temp(i, j) = 0.0_FP
            end do
        end do
        !$omp end do
        !$omp do
        do j = 1, n
            temp(1, j) = 1.0_FP
            prev_temp(1, j) = 1.0_FP
        end do
        !$omp end do
        !$omp do private(b_value)
        do i = 1, n
            b_value = real(n - i, kind=FP)/(n - 1)
            temp(i, 1) = b_value
            prev_temp(i, 1) = b_value
            temp(i, n) = b_value
            prev_temp(i, n) = b_value
        end do
        !$omp end do
    !$omp end parallel

    is_done = .false.
    max_t = 0
    max_diff = 0.0_FP
    !$omp parallel default(none) shared(temp, prev_temp, tmp, n, t_max, max_diff, &
    !$omp& max_t, is_done) private(t, i, j, diff)
    do t = 1, t_max
        !$omp single
            max_diff = 0.0_FP
        !$omp end single

        !$omp barrier
        if (is_done) cycle

        !$omp do collapse(2) reduction(max:max_diff)
        do j = 2, n - 1
            do i = 2, n - 1
                temp(i, j) = 0.25_FP * (prev_temp(i - 1, j) &
                    + prev_temp(i + 1, j) + prev_temp(i, j - 1) &
                    + prev_temp(i, j + 1))
                diff = abs(temp(i, j) - prev_temp(i, j))
                if (diff > max_diff) then
                    max_diff = diff
                end if
            end do
        end do
        !$omp end do

        !$omp single
            max_t = t
            write (unit=error_unit, fmt='(A, I0, A, F12.6)') 'step ', t, ': ', max_diff
            if (max_diff < diff_stop) then
                is_done = .true.
            end if
            tmp => temp
            temp => prev_temp
            prev_temp => tmp
        !$omp end single

        !$omp barrier
    end do
    !$omp end parallel

    call print_system(prev_temp)
    write (*, '(I0, A, F8.6)') max_t, ' steps: ', max_diff

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
