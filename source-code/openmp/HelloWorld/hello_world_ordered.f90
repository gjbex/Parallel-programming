program hello_world
    !$ use omp_lib
    implicit none
    integer :: thread_num = 0, num_threads = 1, i
    
    !$omp parallel default(none) private(thread_num) shared(num_threads)
    !$ num_threads = omp_get_num_threads()
    !$ thread_num = omp_get_thread_num()
    !$omp do ordered
    do i = 0, num_threads - 1
        !$omp ordered
        print '(A, I0)', 'hello from thread ', thread_num
        !$omp end ordered
    end do
    !$omp end do
    !$omp end parallel
    print '(I0, A)', num_threads, ' threads total'

end program hello_world
