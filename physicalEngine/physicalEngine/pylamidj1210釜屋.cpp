#include <Box2D/Box2D.h>
#include "DxLib.h"

//ウィンドウサイズ
#define MIN_X 0
#define MIN_Y 0
#define MAX_X 640
#define MAX_Y 480

//画面の中心座標
#define CENTER_X (MIN_X + MAX_X)/2
#define CENTER_Y (MIN_Y + MAX_Y)/2

//x,y方向の重力
#define GRAVITY_X 0.0
#define GRAVITY_Y 60.0

//box2d関連

//重力加速度設定
b2Vec2 gravity(GRAVITY_X, GRAVITY_Y);
//Box2DWorld
b2World world(gravity, true);

//グラウンドボックスの定義
b2BodyDef groundBodyDef;

//ボディ生成
//x,y：中心座標
//w,h：幅,高さ(円の場合 w：半径)
//angle：角度
//isDynamic：動く物かどうか
//isBall：円かどうか
//density：密度
//restitution：反発力
//friction：摩擦力
b2Body* CreateBody(float x, float y, float w, float h, float angle,
	bool isDynamic, bool isBall, float density, float restitution, float friction)
{
	//ボディ定義
	b2BodyDef bodyDef;
	bodyDef.position.Set(x, y);
	bodyDef.angle = angle;

	if(isDynamic)
		bodyDef.type = b2_dynamicBody;	//動的ボディ
	else
		bodyDef.type = b2_staticBody;	//静的ボディ

	//ボディ作成
	b2Body* body = world.CreateBody(&bodyDef);

	//フィクスチャ定義
	b2FixtureDef fixtureDef;

	//シェイプ作成（trueなら円、falseなら長方形）
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

	fixtureDef.density = density;		 //密度
	fixtureDef.restitution = restitution;//反発力
	fixtureDef.friction = friction;	//摩擦力
	body->CreateFixture(&fixtureDef);
	return body;
}

void MakeWalls()
{

	////壁・床
	b2Body *wall1, *wall2, *floor, *ceiling;
	float w = 20.0, h = 20.0;//幅,高さ(円の場合 w：半径)
	float angle = 0.0;//角度	
	bool isDynamic = false;
	bool isBall = false;
	float density = 0.5;//密度
	float restitution = 0.5;//反発力
	float friction = 0.1;//摩擦力
	floor = CreateBody(CENTER_X, MAX_Y, MAX_X, h, angle, isDynamic, isBall, density, restitution, friction);//床
//	ceiling = CreateBody(CENTER_X, MIN_Y, MAX_X, h, angle, isDynamic, isBall, density, restitution, friction);//天井
	wall1 = CreateBody(MIN_X, CENTER_Y, w, MAX_Y, angle, isDynamic, isBall, density, restitution, friction);//壁
	wall2 = CreateBody(MAX_X, CENTER_Y, w, MAX_Y, angle, isDynamic, isBall, density, restitution, friction);//壁

}

void MakeObjects()
{
	float x, y;//中心座標
	float w = 20.0, h = 20.0;//幅,高さ(円の場合 w：半径)
	float angle = 0.0;//角度
	bool isDynamic = true;
	bool isBall = false;
	float density = 10.0;//密度
	float restitution = 0.5;//反発力
	float friction = 0.5;//摩擦力(0～1.00)

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
		//形状別に処理をする
		b2Fixture *fixture = body->GetFixtureList();
		//描画
		//矩形
		if(fixture->GetType() == b2Shape::e_polygon)
		{
			b2PolygonShape *poly = (b2PolygonShape*)fixture->GetShape();
			b2Vec2 vert[b2_maxPolygonVertices];
			int vertNum = poly->GetVertexCount();
			//変換行列を取得する
			b2Transform transformB = body->GetTransform();
			//矩形の制御点を取得する
			for(int i=0; i<vertNum; ++i){
				vert[i] = poly->m_vertices[i];
				//変換行列で制御点を変換する
				vert[i] = b2Mul(transformB, vert[i]);
			}
			//多角形
			for( int i = 0; i < vertNum; i++ )
			{
				DrawLine(vert[i].x, vert[i].y, vert[(i+1)%vertNum].x, vert[(i+1)%vertNum].y, GetColor(0,255,255));
			}
		}
		
		//円
		else if( fixture->GetType() == b2Shape::e_circle )
		{
			b2CircleShape *poly = (b2CircleShape *)fixture->GetShape();
			//変換行列を取得する
			b2Transform transformB = body->GetTransform();
			b2Vec2 center = b2Mul(transformB,poly->m_p);

			float r = poly->m_radius;//半径;
			DrawCircle(center.x, center.y, r, GetColor(255,0,255), FALSE);
		}
	}
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )

{
	ChangeWindowMode(true);
	if( DxLib_Init() == -1 )//ＤＸライブラリ初期化処理
		return -1;//エラーが起きたら直ちに終了

	//描画先を裏画面に設定
	SetDrawScreen( DX_SCREEN_BACK );

	//床と壁を作る
	MakeWalls();
	//物体生成
	MakeObjects();


	//物理計算用変数
	//更新速度
	float32 timeStep = 1.0f / 60.0f;
	//反復数
	int32 velocityIterations = 10;
	int32 positionIterations = 10;


	//Escキーで終了
	while(!ProcessMessage() && !CheckHitKey(KEY_INPUT_ESCAPE))
	{
		//画面を初期化
		ClearDrawScreen();

		//物理計算
		world.Step(timeStep, velocityIterations, positionIterations);
		world.ClearForces();

		//物体の描画
		DrawObjects();

		//裏画面の内容を表画面に反映
		ScreenFlip();
	}

	DxLib_End();//ＤＸライブラリ使用の終了処理

	return 0;//ソフトの終了 
}