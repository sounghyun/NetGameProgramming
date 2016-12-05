#ifndef GUARDIAN
#define GUARDIAN

#include "guardian.h"

void Guardian::createguardian(float angle)
{
	this->hp = 20;
	this->Sangle = angle;
	this->angle = 90 + angle;
	if (this->Sangle)
		this->z = -460;
	else
		this->z = -40;
	this->x = 100;
	this->y = 0;
	this->exist = true;
	this->time = 250;
}

void Guardian::ranberguardian()
{
	glPushMatrix();
	if (&this->Rangle < 0)
		glTranslated(this->x, this->y + this->Rangle * 0.02, this->z);
	else
		glTranslated(this->x, this->y + this->Langle * 0.02, this->z);

	glRotated(this->angle, 0, 1, 0);

	glPushMatrix();			//�Ӹ�
	if (this->Sangle)
		glColor3d(0, 0, 1);
	else
		glColor3d(1, 0, 0);
	glTranslated(0, 15, 0);
	glutSolidSphere(2, 30, 30);
	glPopMatrix();

	glPushMatrix();			//����
	glTranslated(0, 10, 0);
	glScaled(1, 2, 0.7);
	glutSolidCube(3.5);
	glPopMatrix();

	glPushMatrix();			//������
	glTranslated(-2, 12.5, 0);
	glScaled(1, 1, 1);
	glutSolidSphere(0.75, 30, 30);		//���

	glRotated(-this->Rangle, 1, 0, 0);

	glRotated(-30, 0, 0, 1);

	glTranslated(0, -1.5, 0);
	glScaled(1, 3, 1);
	glutSolidCube(1);							//��
	glPopMatrix();


	glPushMatrix();			//��������
	glTranslated(2, 12.5, 0);
	glScaled(1, 1, 1);
	glutSolidSphere(0.75, 30, 30);		//���

	glRotated(-this->Langle, 1, 0, 0);

	glRotated(30, 0, 0, 1);

	glTranslated(0, -1.5, 0);
	glScaled(1, 3, 1);
	glutSolidCube(1);							//��
	glPopMatrix();

	glPushMatrix();			//���� ��ü
	glTranslated(-0.75, 6, 0);
	glutSolidSphere(0.75, 30, 30);	//����1

	glRotated(-this->Langle, 1, 0, 0);

	glTranslated(0, -1.5, 0);
	glScaled(1, 2, 1);
	glutSolidCube(1);						//�ٸ�1
	glScaled(1, 0.5, 1);

	glTranslated(0, -1.5, 0);
	glutSolidSphere(0.75, 30, 30);	//����2

	glRotated(this->Langle, 1, 0, 0);

	glTranslated(0, -1, 0);
	glScaled(1, 2, 1);
	glutSolidCube(1);						//�ٸ�2
	glScaled(1, 0.5, 1);

	glTranslated(0, -1, 0.5);
	glScaled(2, 1, 3);
	glutSolidSphere(0.5, 30, 30);		//��
	glPopMatrix();

	glPushMatrix();			//������ ��ü
	glTranslated(0.75, 6, 0);
	glutSolidSphere(0.75, 30, 30);	//����1

	glRotated(-this->Rangle, 1, 0, 0);

	glTranslated(0, -1.5, 0);
	glScaled(1, 2, 1);
	glutSolidCube(1);						//�ٸ�1
	glScaled(1, 0.5, 1);

	glTranslated(0, -1.5, 0);
	glutSolidSphere(0.75, 30, 30);	//����2

	glRotated(this->Rangle, 1, 0, 0);

	glTranslated(0, -1, 0);
	glScaled(1, 2, 1);
	glutSolidCube(1);						//�ٸ�2
	glScaled(1, 0.5, 1);

	glTranslated(0, -1, 0.5);
	glScaled(2, 1, 3);
	glutSolidSphere(0.5, 30, 30);		//��
	glPopMatrix();

	glPopMatrix();
}

#endif // !GUARDIAN