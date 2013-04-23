domains
  n = integer
  m = integer*
predicates
  evenmember(n,m)
clauses
  evenmember(Name,[Name | _]) 
    if Nargh  = Name / 2 and
       Nyargh = round(Nargh) and
       Nargh - Nyargh = 0.
  evenmember(Name,[_|Tail]) if 
   	evenmember(Name,Tail).