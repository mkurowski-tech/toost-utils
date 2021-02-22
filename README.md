A few utils that was helpful for me

make_string(T ... args)	- making string in place from values of primary types

are_same<T ... >	- like is_same but for many types
  
sequence(T v)		- to short loops, for(auto i : sequence(6))

series(T src, T dst)	- to short loops iterating over range, for(auto c : series('a','z'))

randvector<T>		- vector with getting random element
  
vectorfile<T>		- vector with writting and reading a file

and math

strong, tenpower and random permutation
