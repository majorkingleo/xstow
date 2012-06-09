BEGIN{
  print_it = 1;
}

/^#ifdef/{
  if( match( defines, $2 ) )
  {
     print_it = 1;
  } else {
     printf( "%s:%d:#ifdef %s is undefined\n", FILENAME, FNR, $2 ) > "/dev/stderr";
     print_it = 0;
  }
}

{
   if( print_it && !match( $0, /^#/ ) )
      print $0;
   line++;   
}

/^#endif/{
  print_it = 1;
}

/^#else/{
  if( print_it )
    print_it = 0;
  else
    print_it = 1;
}

