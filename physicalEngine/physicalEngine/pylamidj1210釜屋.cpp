#include <Box2D/Box2D.h>
#include "DxLib.h"

//�E�B���h�E�T�C�Y
#define MIN_X 0
#define MIN_Y 0
#define MAX_X 640
#define MAX_Y 480

//��ʂ̒��S���W
#define CENTER_X (MIN_X + MAX_X)/2
#define CENTER_Y (MIN_Y + MAX_Y)/2

//x,y�����̏d��
#define GRAVITY_X 0.0
#define GRAVITY_Y 60.0

//box2d�֘A

//�d�͉����x�ݒ�
b2Vec2 gravity(GRAVITY_X, GRAVITY_Y);
//Box2DWorld
b2World world(gravity, true);

//�O���E���h�{�b�N�X�̒�`
b2BodyDef groundBodyDef;

//�{�f�B����
//x,y�F���S���W
//w,h�F��,����(�~�̏ꍇ w�F���a)
//angle�F�p�x
//isDynamic�F���������ǂ���
//isBall�F�~���ǂ���
//density�F���x
//restitution�F������
//friction�F���C��
b2Body* CreateBody(float x, float y, float w, float h, float angle,
	bool isDynamic, bool isBall, float density, float restitution, float friction)
{
	//�{�f�B��`
	b2BodyDef bodyDef;
	bodyDef.position.Set(x, y);
	bodyDef.angle = angle;

	if(isDynamic)
		bodyDef.type = b2_dynamicBody;	//���I�{�f�B
	else
		bodyDef.type = b2_staticBody;	//�ÓI�{�f�B

	//�{�f�B�쐬
	b2Body* body = world.CreateBody(&bodyDef);

	//�t�B�N�X�`����`
	b2FixtureDef fixtureDef;

	//�V�F�C�v�쐬�itrue�Ȃ�~�Afalse�Ȃ璷���`�j
	b2CircleShape dynamicBall;
	dynamicBall.m_radius = w;

	b2CircleShape staticBall;
	staticBall.m_radius = w;

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(w, h);

	b2PolygonShape staticBox;
	staticBox.SetAsBox(w, h);

	if(isDynamic)
	{
		if(isBall)
			fixtureDef.shape = &dynamicBall;
		else
			fixtureDef.shape = &dynamicBox;
	}
	else
	{
		if(isBall)
			fixtureDef.shape = &staticBall;
		else
			fixtureDef.shape = &staticBox;
	}

	fixtureDef.density = density;		 //���x
	fixtureDef.restitution = restitution;//������
	fixtureDef.friction = friction;	//���C��
	body->CreateFixture(&fixtureDef);
	return body;
}

void MakeWalls()
{

	////�ǁE��
	b2Body *wall1, *wall2, *floor, *ceiling;
	float w = 20.0, h = 20.0;//��,����(�~�̏ꍇ w�F���a)
	float angle = 0.0;//�p�x	
	bool isDynamic = false;
	bool isBall = false;
	float density = 0.5;//���x
	float restitution = 0.5;//������
	float friction = 0.1;//���C��
	floor = CreateBody(CENTER_X, MAX_Y, MAX_X, h, angle, isDynamic, isBall, density, restitution, friction);//��
//	ceiling = CreateBody(CENTER_X, MIN_Y, MAX_X, h, angle, isDynamic, isBall, density, restitution, friction);//�V��
	wall1 = CreateBody(MIN_X, CENTER_Y, w, MAX_Y, angle, isDynamic, isBall, density, restitution, friction);//��
	wall2 = CreateBody(MAX_X, CENTER_Y, w, MAX_Y, angle, isDynamic, isBall, density, restitution, friction);//��

}

void MakeObjects()
{
	float x, y;//���S���W
	float w = 20.0, h = 20.0;//��,����(�~�̏ꍇ w�F���a)
	float angle = 0.0;//�p�x
	bool isDynamic = true;
	bool isBall = false;
	float density = 10.0;//���x
	float restitution = 0.5;//������
	float friction = 0.5;//���C��(0�`1.00)

	b2Body *body;
	x = CENTER_X, y = 0;
	b2Body* CreateBody(float x, float y, float w, float h, float angle, bool isDynamic, bool isBall, float density, float restitution, float friction);
		for(int i=0; i<36; i++){
			if(i<11)
			body = CreateBody(100+40*i, MAX_Y-40, w, h, angle, isDynamic, isBall, density, restitution, friction);
			else if(i>=11 && i<20)
			body = CreateBody(140+40*(i-11), MAX_Y-80, w, h, angle, isDynamic, isBall, density, restitution, friction);
			else if(i>=20 && i<27)
			body = CreateBody(180+40*(i-20), MAX_Y-120, w, h, angle, isDynamic, isBall, density, restitution, friction);
			else if(i>=27 && i<32)
			body = CreateBody(220+40*(i-27), MAX_Y-160, w, h, angle, isDynamic, isBall, density, restitution, friction);
			else if(i>=32 && i<35)
			body = CreateBody(260+40*(i-32), MAX_Y-200, w, h, angle, isDynamic, isBall, density, restitution, friction);
			else
			body = CreateBody(300, MAX_Y-240, w, h, angle, isDynamic, isBall, density, restitution, friction);
	}
	body = CreateBody(MAX_X/2, 0, w, h, angle, isDynamic, !isBall, density*100, restitution, friction);
}

void DrawObjects()
{

	for(b2Body *body = world.GetBodyList(); body!=NULL; body = body->GetNext() )
	{
		//�`��ʂɏ���������
		b2Fixture *fixture = body->GetFixtureList();
		//�`��
		//��`
		if(fixture->GetType() == b2Shape::e_polygon)
		{
			b2PolygonShape *poly = (b2PolygonShape*)fixture->GetShape();
			b2Vec2 vert[b2_maxPolygonVertices];
			int vertNum = poly->GetVertexCount();
			//�ϊ��s����擾����
			b2Transform transformB = body->GetTransform();
			//��`�̐���_���擾����
			for(int i=0; i<vertNum; ++i){
				vert[i] = poly->m_vertices[i];
				//�ϊ��s��Ő���_��ϊ�����
				vert[i] = b2Mul(transformB, vert[i]);
			}
			//���p�`
			for( int i = 0; i < vertNum; i++ )
			{
				DrawLine(vert[i].x, vert[i].y, vert[(i+1)%vertNum].x, vert[(i+1)%vertNum].y, GetColor(0,255,255));
			}
		}
		
		//�~
		else if( fixture->GetType() == b2Shape::e_circle )
		{
			b2CircleShape *poly = (b2CircleShape *)fixture->GetShape();
			//�ϊ��s����擾����
			b2Transform transformB = body->GetTransform();
			b2Vec2 center = b2Mul(transformB,poly->m_p);

			float r = poly->m_radius;//���a;
			DrawCircle(center.x, center.y, r, GetColor(255,0,255), FALSE);
		}
	}
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )

{
	ChangeWindowMode(true);
	if( DxLib_Init() == -1 )//�c�w���C�u��������������
		return -1;//�G���[���N�����璼���ɏI��

	//�`���𗠉�ʂɐݒ�
	SetDrawScreen( DX_SCREEN_BACK );

	//���ƕǂ����
	MakeWalls();
	//���̐���
	MakeObjects();


	//�����v�Z�p�ϐ�
	//�X�V���x
	float32 timeStep = 1.0f / 60.0f;
	//������
	int32 velocityIterations = 10;
	int32 positionIterations = 10;


	//Esc�L�[�ŏI��
	while(!ProcessMessage() && !CheckHitKey(KEY_INPUT_ESCAPE))
	{
		//��ʂ�������
		ClearDrawScreen();

		//�����v�Z
		world.Step(timeStep, velocityIterations, positionIterations);
		world.ClearForces();

		//���̂̕`��
		DrawObjects();

		//����ʂ̓��e��\��ʂɔ��f
		ScreenFlip();
	}

	DxLib_End();//�c�w���C�u�����g�p�̏I������

	return 0;//�\�t�g�̏I�� 
}