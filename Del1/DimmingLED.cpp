/*
 * GccApplication1.cpp
 *
 * Created: 19.10.2022 08:49:20
 * Author : hakon
 */ 


#include<util./delay.h>
#include <avr/io.h>
#include<math.h>
#define F_CPU 1000000UL
float percivedLight(float mL)
{
	float pL=100.0*sqrtf(mL/100.0);
	
	return pL;
	
}

void dimUp()
{
	//PORTC|=(1<<PC5);
	float mLp=0.0;
	
	for (int i= 0; i<100; i++) //from 0 to 100%
	{
		
		float onTime=0.0;
		float offTime=100.0;
		float pLp=percivedLight(mLp);
		PORTC|=(1<<PC5);
		
		while(onTime<pLp)
		{
			_delay_ms(0.03);
			onTime=onTime+1.0;
		}
		
		PORTC&=~(1<<PC5);
		
		while(offTime>pLp)
		{
			_delay_ms(0.03);
			offTime=offTime-1.0;
		}
		
			mLp=mLp+1.0;
		
		
	}
	
	
}

void dimDown()
{
	
	//PORTC|=(1<<PC5);
	float mLp=99.0;
	
	for (int i= 99; i>0; i--) //from 0 to 100%
	{
		
		float onTime=0.0;
		float offTime=100.0;
		float pLp=percivedLight(mLp);
		PORTC|=(1<<PC5);
		
		while(onTime<pLp)
		{
			_delay_ms(0.03);
			onTime=onTime+1.0;
		}
		
		PORTC&=~(1<<PC5);
		
		while(offTime>pLp)
		{
			_delay_ms(0.03);
			offTime=offTime-1.0;
		}
	
			mLp=mLp-1.0;
		
	}
	
	
}




int main(void)
{
	/* Replace with your application code */
	
	DDRC=(1<<PC5);
	
	while (1)
	{
		dimUp();
		dimDown();
	}
}
