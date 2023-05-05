program pi
    use, intrinsic :: iso_fortran_env, only : DP => REAL64
    implicit none
    real(kind=DP) :: pi_value = 0.0_DP, dx, x, partial_pi
    integer :: i, nr_points = 10000
    character(len=1024) :: buffer

    if (command_argument_count() > 1) then
        call get_command_argument(1, buffer)
        read (buffer, *) nr_points
    end if

    dx = 1.0_DP/nr_points

    !$omp parallel default(none) shared(pi_value, dx, nr_points) private(x, partial_pi)
    !$omp do
    do i = 0, nr_points - 1
        x = i*dx
        partial_pi = partial_pi + 1.0_DP/(1.0_DP + x**2)
    end do
    !$omp end do
    !$omp critical
    pi_value = pi_value + partial_pi
    !$omp end critical
    !$omp end parallel

    pi_value = 4.0_DP*dx*pi_value

    print '(A, F15.8)', 'pi = ', pi_value

end program pi
