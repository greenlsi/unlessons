/* Ejercicio 6*/

byte timer=0;
byte button=0;
byte presencia=0;
byte alarma_EN=0;

/*estado 0 es luz off, estado 1 es luz on y estado 2 es alarma on*/
byte state;

active proctype light(){
	state=0;
	do
	:: (state==0) -> atomic{
		if
		:: button -> state=1; printf("enciende"); button=0;
		:: presencia&&(!alarma_EN) -> state=1; printf("enciende"); presencia=0;
		:: presencia&&alarma_EN -> state=2; printf("enciende"); printf("alarma"); presencia=0;
		fi
		}
	:: (state==1) -> atomic{
		if
		:: button&&!timer -> state=0; printf("apaga"); button=0;
		:: timer&&!button -> state=0; printf("apaga"); timer=0;
		:: timer&&button -> state=0; printf("apaga"); button=0; timer=0;
		:: (presencia&&alarma_EN&&!button&&!timer) -> state=2; printf("enciende"); printf("alarma"); presencia=0;
		:: (presencia&&!alarma_EN&&!button&&!timer) -> state=1; printf("enciende"); presencia=0;
		fi
		}
	:: (state==2) -> atomic{
		if
		:: !alarma_EN -> state=0; printf("fin alarma");
		fi
		}
	
	od
}

active proctype entorno(){
	do
	::true -> timer=1;
	::true -> button=1  
	::true -> presencia=1
	::true -> alarma_EN=1
	::true -> alarma_EN=0
	::true -> skip
	od
}

ltl spec1{
	[]( ((state==1)&&(button)) -> <>(state==0))
}

ltl spec2{
	[]( (state==1&&button) -> <>(state==0))
}

ltl spec3{
	[]( (state==1&&timer) -> <>(state==0))
}

ltl spec4{
	[]( ((state==1)&&(presencia&&alarma_EN&&!button&&!timer))) -> <>(state==2))
}

ltl spec5{
	[]( ((state==1)&&(presencia&&!alarma_EN&&!button&&!timer)) -> <>(state==1))
}

ltl spec6{
	[]( ((state==2)&&!alarma_EN) -> <>(state==0))
}