module precision_kinds

    use, intrinsic :: iso_fortran_env, only : FP => REAL32
    implicit none

end module precision_kinds

program heat

    implicit none
    integer, parameter :: DEFAULT_GRID_SIZE = 10
    integer, parameter :: DEFAULT_MAX_STEPS = 5
    integer, parameter :: MIN_GRID_SIZE = 2
    integer, parameter :: MIN_MAX_STEPS = 1
    integer :: n, t_max, parse_status
    logical :: print_solution

    n = DEFAULT_GRID_SIZE
    t_max = DEFAULT_MAX_STEPS
    print_solution = .false.

    parse_status = parse_command_line_arguments(n, t_max, print_solution)
    if (parse_status == 1) then
        stop 1
    end if
    if (parse_status == 2) then
        call run_simulation(n, t_max, print_solution)
    end if

contains

    subroutine run_simulation(n, t_max, print_solution)
        use precision_kinds, only : FP
        use, intrinsic :: iso_fortran_env, only : error_unit
        implicit none
        integer, intent(in) :: n, t_max
        logical, intent(in) :: print_solution
        real(kind=FP), parameter :: diff_stop = 1e-3_FP
        real(kind=FP), dimension(:, :), allocatable, target :: temp_data
        real(kind=FP), dimension(:, :), allocatable, target :: prev_temp_data
        real(kind=FP), dimension(:, :), pointer :: temp, prev_temp, tmp
        real(kind=FP) :: diff, max_diff, b_value
        integer :: istat, i, j, max_t, t
        logical :: is_done

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

            !$omp do
            do i = 1, n
                b_value = real(n - i, kind=FP) / (n - 1)
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
        !$omp parallel default(none) shared(temp, prev_temp, tmp, n, t_max, &
        !$omp& max_diff, max_t, is_done) private(t, i, j, diff)
        do t = 1, t_max
            !$omp single
                max_diff = -huge(max_diff)
            !$omp end single

            !$omp barrier
            if (is_done) cycle

            !$omp do collapse(2) reduction(max:max_diff)
            do j = 2, n - 1
                do i = 2, n - 1
                    temp(i, j) = 0.25_FP * (prev_temp(i - 1, j) + &
                                            prev_temp(i + 1, j) + &
                                            prev_temp(i, j - 1) + &
                                            prev_temp(i, j + 1))
                    diff = abs(temp(i, j) - prev_temp(i, j))
                    if (diff > max_diff) then
                        max_diff = diff
                    end if
                end do
            end do
            !$omp end do

            !$omp single
                max_t = t
                write (unit=error_unit, fmt='(A, I0, A, F12.6)') &
                    'step ', t, ': ', max_diff
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

        if (print_solution) then
            call print_system(prev_temp)
        end if
        write (*, '(I0, A, F8.6)') max_t, ' steps: ', max_diff

        deallocate(temp_data)
        deallocate(prev_temp_data)
    end subroutine run_simulation

    integer function parse_command_line_arguments(n, t_max, print_solution)
        use, intrinsic :: iso_fortran_env, only : error_unit
        implicit none
        integer, intent(inout) :: n, t_max
        logical, intent(inout) :: print_solution
        integer :: arg_count, arg_index
        character(len=1024) :: argument

        arg_count = command_argument_count()
        arg_index = 1

        do while (arg_index <= arg_count)
            call get_command_argument(arg_index, argument)
            select case (trim(argument))
                case ('-h')
                    call print_usage()
                    parse_command_line_arguments = 0
                    return
                case ('-n')
                    if (.not. parse_integer_option(arg_index, &
                                                   arg_count, &
                                                   'grid size', &
                                                   MIN_GRID_SIZE, &
                                                   n)) then
                        call print_usage()
                        parse_command_line_arguments = 1
                        return
                    end if
                case ('-t')
                    if (.not. parse_integer_option(arg_index, &
                                                   arg_count, &
                                                   'maximum number of time steps', &
                                                   MIN_MAX_STEPS, &
                                                   t_max)) then
                        call print_usage()
                        parse_command_line_arguments = 1
                        return
                    end if
                case ('-s')
                    print_solution = .true.
                case default
                    write (unit=error_unit, fmt='(3A)') &
                        'error: unexpected argument ''', trim(argument), ''''
                    call print_usage()
                    parse_command_line_arguments = 1
                    return
            end select
            arg_index = arg_index + 1
        end do

        parse_command_line_arguments = 2
    end function parse_command_line_arguments

    logical function parse_integer_option(arg_index, &
                                          arg_count, &
                                          option_name, &
                                          min_value, &
                                          result)
        use, intrinsic :: iso_fortran_env, only : error_unit
        implicit none
        integer, intent(inout) :: arg_index
        integer, intent(in) :: arg_count, min_value
        integer, intent(out) :: result
        character(len=*), intent(in) :: option_name
        integer :: io_status
        character(len=1024) :: argument

        parse_integer_option = .false.
        if (arg_index >= arg_count) then
            write (unit=error_unit, fmt='(2A)') &
                'error: missing value for ', trim(option_name)
            return
        end if

        arg_index = arg_index + 1
        call get_command_argument(arg_index, argument)
        read (argument, *, iostat=io_status) result
        if (io_status /= 0) then
            write (unit=error_unit, fmt='(3A)') &
                'error: invalid ', trim(option_name), ' value'
            return
        end if
        if (result < min_value) then
            write (unit=error_unit, fmt='(A, A, A, I0)') &
                'error: ', trim(option_name), ' must be >= ', min_value
            return
        end if

        parse_integer_option = .true.
    end function parse_integer_option

    subroutine print_usage()
        use, intrinsic :: iso_fortran_env, only : error_unit
        implicit none

        write (unit=error_unit, fmt='(A)') &
            'Usage: heat_f90.exe [-n grid_size] [-t max_steps] [-s]'
        write (unit=error_unit, fmt='(A, I0)') &
            '  -n grid_size  Grid dimension, integer >= ', MIN_GRID_SIZE
        write (unit=error_unit, fmt='(A, I0)') &
            '  -t max_steps  Maximum number of time steps, integer >= ', &
            MIN_MAX_STEPS
        write (unit=error_unit, fmt='(A)') &
            '  -s            Print the final temperature matrix'
        write (unit=error_unit, fmt='(A)') &
            '  -h            Show this help message'
    end subroutine print_usage

    subroutine print_system(temp)
        use precision_kinds, only : FP
        implicit none
        real(kind=FP), dimension(:, :), intent(in) :: temp
        integer :: i

        do i = 1, size(temp, 1)
            print '(*(F12.6))', temp(i, :)
        end do
    end subroutine print_system

end program heat
