program compute_pi
    use, intrinsic :: iso_fortran_env, only : DP => REAL64
    implicit none
    integer :: n = 10000000, i
    real(kind=DP) :: total, x, delta
    character(len=32) :: buffer

    if (command_argument_count() > 0) then
        call get_command_argument(1, buffer)
        read(buffer, *) n
    end if
    total = 0.0_DP
    delta = 1.0_DP/real(n, kind=DP)
    do i = 0, n - 1
        x = (real(i, kind=DP) + 0.5_DP)*delta
        total = total + 4.0_DP/(1.0_DP + x**2)
    end do
    total = total*delta
    print '(A, F20.18)', 'PI = ', total
end program compute_pi
