program hello_world
    implicit none
    integer :: thread_num = 0, num_threads = 1
    
    print '(A, I0)', 'hello from thread ', thread_num
    print '(I0, A)', num_threads, ' threads total'

end program hello_world
