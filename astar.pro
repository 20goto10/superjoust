/* A&.PRO - The A-Ampersand Search Algorithm */
/* (C) Copyright 1995, by Ben Chadwick */
/* Based very loosely on Steve L. "Mendacious Monkey" Tanimoto's A* search */
domains
  city = symbol
  citylist = city*
  number = integer
  numtype = symbol
database       	/* This is as close as Prolog comes to having global vars. */
  is_equalto(numtype,number)
predicates
  path(city,city,number) /* number = distance */
  longitude(city,number) /* number = longitude */
  route(city,city,citylist,number) 
                          /* Route to follow, citylist= rooms already seen. */
  search(city,city)       /* the A-Ampersand search */
  neighborroom(city,city)
  member(city,citylist)
clauses
  path(brie,ren,244).  /* here goes... */
  path(ren,can,176).
  path(ren,paris,348).
  path(ren,brie,244).
  path(ren,nan,107).
  path(can,cal,120).
  path(can,paris,241).  
  path(can,ren,176).
  path(cal,mary,534).
  path(cal,can,120).
  path(cal,paris,297).
  path(mary,stras,145).
  path(mary,dijon,201).
  path(mary,cal,372).
  path(mary,paris,534).  
  path(stras,dijon,335).
  path(stras,mary,145).
  path(dijon,stras,335).
  path(dijon,lyon,192).
  path(dijon,paris,313).
  path(dijon,mary,201).
  path(lyon,gren,104).
  path(lyon,av,216).
  path(lyon,lim,389).
  path(lyon,dijon,192).
  path(gren,av,227).
  path(gren,lyon,104).
  path(av,gren,227).
  path(av,mars,99).
  path(av,mont,91).
  path(av,lyon,216).
  path(mars,nice,188).
  path(mars,av,99).
  path(nice,mars,188).
  path(mont,av,91).
  path(mont,to,240).
  path(to,mont,240).
  path(to,bord,253).
  path(to,lim,313).
  path(bord,lim,220).
  path(bord,to,253).
  path(bord,nan,329).
  path(lim,lyon,389).
  path(lim,to,313).
  path(lim,bord,220).
  path(lim,nan,329).
  path(lim,paris,396).
  path(nan,lim,329).
  path(nan,bord,329).
  path(nan,ren,107).
  path(paris,cal,297).
  path(paris,mary,372).
  path(paris,dijon,313).
  path(paris,lim,396).
  path(paris,ren,348).
  path(paris,can,241).  /* Damn Europe for having so many cities! */
  
  neighborroom(X,Y) if path(X,Y,_).        /* Are they neighbors? */
  
  search(X,X).                             /* don't waste time. */
  search(Here,There) if                       
     assertz(is_equalto(oldbest,23000)),   /* set Best score too high */
     route(There,Here,[There],0)           /* Start with current dist=0 */
     or 1 = 1.   			   /* 1=1, forced yes: without this it
				           will always be No due to "fail." 
				           This search *always* works so it
				           should always be Yes. */
				           
  route(Room,Room,VisitedRooms,Dist) if     /* Arrived at finish line. */
    is_equalto(oldbest,Oldbest) and         /* Get last score into Oldbest */
    Dist < Oldbest and                      /* Is this a better route ? */
    retract(is_equalto(oldbest,Oldbest)) and  /* Remove old setting */
    asserta(is_equalto(oldbest,Dist)) and     /* Establish new setting */
    write(VisitedRooms),nl,                 /* Output. */
    fail.				    /* This forces a No- see above. */
    
  route(Room,Finish,VisitedRooms,Distance):- /* Anything but finish line. */
    path(Room,Nextroom,Z),                   /* Get dist. between cities. */
    NDistance = Distance + Z,           /* New Distance */
    not(member(NextRoom,VisitedRooms)), /* haven't been there. */
    route(NextRoom,Finish,[NextRoom|VisitedRooms],NDistance).
    
  member(X,[X|_]).                         /* Checks if X is in a list. */
  member(X,[_|H]) if member(X,H).          /* recurse. */