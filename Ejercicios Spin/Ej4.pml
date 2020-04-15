/* Ejercicio 4*/

byte button=0;

/*estado 0 es luz off y estado 1 es luz on*/
byte state;

active proctype light(){
	state=0;
	do
	:: (state==0) -> atomic{
		if
		:: (button) -> state=1; printf("enciende"); button=0;
		fi
		}
	:: (state==1) -> atomic{
		if
		:: (button) -> state=0; printf("apaga"); button=0;
		fi
		}
	od
}

active proctype entorno(){
	do
	::true -> button=1
	::true -> skip
	od;
}

ltl spec1{
	[]( ((state==0)&&button) -> <> (state==1))
}

ltl spec2{
	[]( ((state==1)&&button) -> <> (state==0))
}