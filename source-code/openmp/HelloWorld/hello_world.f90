program hello_world
    !$ use omp_lib
    implicit none
    integer :: thread_num = 0, num_threads = 1
    
    !$omp parallel default(none) private(thread_num) shared(num_threads)
    !$ thread_num = omp_get_thread_num()
    print '(A, I0)', 'hello from thread ', thread_num
    !$ if (thread_num == 0) num_threads = omp_get_num_threads()
    !$omp end parallel
    print '(I0, A)', num_threads, ' threads total'

end program hello_world
