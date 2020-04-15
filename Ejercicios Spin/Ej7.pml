/* Ejercicio 7*/

byte timer=0;
byte button=0;
byte presencia=0;
byte alarma_EN=0;

int code_1=0;
int code_2=0;
int code_3=0;

int pass_1=5;
int pass_2=3;
int pass_3=8;

int count=0;

/*estado 0 es luz off, estado 1 es luz on, estado 2 es alarma on y estado 3 es introduccion de codigo*/
byte state;

active proctype light(){
	state=0;
	do
	:: (state==0) -> atomic{
		if
		:: button -> state=1; printf("enciende"); button=0;
		:: presencia&&(!alarma_EN) -> state=1; printf("enciende"); presencia=0;
		:: presencia&&alarma_EN -> state=2; printf("enciende"); printf("alarma"); presencia=0; count=0; code_1=0; code_2=0; code_3=0;
		fi
		}
	:: (state==1) -> atomic{
		if
		:: button&&!timer -> state=0; printf("apaga"); button=0;
		:: timer&&!button -> state=0; printf("apaga"); timer=0;
		:: timer&&button -> state=0; printf("apaga"); button=0; timer=0;
		:: (presencia&&alarma_EN&&!button&&!timer) -> state=2; printf("enciende"); printf("alarma"); presencia=0; count=0; code_1=0; code_2=0; code_3=0;
		:: (presencia&&!alarma_EN&&!button&&!timer) -> state=1; printf("enciende"); presencia=0;
		fi
		}
	:: (state==2) -> atomic{
		if
		:: !alarma_EN -> state=0; printf("fin alarma");
		:: button&&count==0 -> state=3; printf("introducir digito 1"); code_1++;
		:: button&&count==1 -> state=3; printf("introducir digito 2"); code_2++;
		:: button&&count==2 -> state=3; printf("introducir digito 3"); code_3++;
		fi
		}
	:: (state==3) -> atomic{
		if
		:: button&&count==0 -> state=3; printf("pulsado"); button=0; code_1++;
		:: button&&count==1 -> state=3; printf("pulsado"); button=0; code_2++;
		:: button&&count==2 -> state=3; printf("pulsado"); button=0; code_3++;
		:: timer&&count<3 -> state=2; printf("digito introducido"); count++;
		:: timer&&count>=3&&code_1==pass_1&&code_2==pass_2&&code_3==pass_3 -> state=0; printf("fin alarma"); count=0; code_1=0; code_2=0; code_3=0;
		:: timer&&count>=3&&(code_1!=pass_1||code_2!=pass_2||code_3!=pass_3) -> state=2; printf("codigo incorrecto"); count=0; code_1=0; code_2=0; code_3=0;
		fi
		}
	od
}

active proctype entorno(){
	do
	::true -> timer=1
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
	[]( ((state==1)&&(presencia&&alarma_EN&&!button&&!timer)) -> <>(state==2))
}

ltl spec5{
	[]( ((state==1)&&(presencia&&!alarma_EN&&!button&&!timer)) -> <>(state==1))
}

ltl spec6{
	[]( ((state==2)&&!alarma_EN) -> <>(state==0))
}

ltl spec7{
	[]( ((state==3)&&count>=3&&code_1==pass_1&&code_2==pass_2&&code_3==pass_3) -> <>(state==2))
}

ltl spec8{
	[]( ((state==3)&&count>=3&&(code_1!=pass_1||code_2!=pass_2||code_3!=pass_3)) -> <>(state==3))
}