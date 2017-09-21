/*
Box2Dゲーム
スペースキーで出るピンクのボールを白いゴールへ絵を描いて導く
Zキーですべての物体を消去しステージの最初から始める
*/

#include <Box2D/Box2D.h>
#include "DxLib.h"
#define _USE_MATH_DEFINES
#include <math.h>


//ウィンドウサイズ
#define MIN_X 0
#define MIN_Y 0
#define MAX_X 960
#define MAX_Y 720

//画面の中心座標
#define CENTER_X ((MIN_X + MAX_X)/2)
#define CENTER_Y ((MIN_Y + MAX_Y)/2)

//x,y方向の重力
#define GRAVITY_X 0.0
#define GRAVITY_Y 9.8

//スケール変換
#define SCALE 0.01//1ピクセルが何メートルか

//お絵かき用
#define MAX_OBJECT 10000
#define MAX_POS 500
#define PEN_W 3

//ゲーム用
#define BALL_SIZE 25//ボールの半径
#define GOAL_SIZE 25//ゴールの半径
#define STAGE_NUM 3//ステージ数

//マウスポインタ等の座標
struct Pos{
	int x;
	int y;
};

struct Object{
	Pos pos[MAX_POS];//マウスポインタの座標
	int color;
	int p_num;
	b2Body *DrawBody;
};

struct Object object[MAX_OBJECT];

struct Stage{
	b2Body *body;
	Pos start;
	Pos goal;
	Pos end;
};

struct Stage stage[STAGE_NUM];

int o_num = 0;//物体の数
int x,y;//マウスポインタの座標
int lx,ly;//1フレーム前のマウスポインタの座標
bool isDraw = false;//手描き中かどうか
bool isBall = false;//ボールがあるかどうか
int stage_num=0;//ステージ数

//box2d関連

//重力加速度設定
b2Vec2 gravity(GRAVITY_X, GRAVITY_Y);
//Box2DWorld
b2World world(gravity, true);

//groundbody
b2BodyDef groundbodyDef;
b2Body *groundbody;

//ボール
b2Body *ball;

//ゲームオーバーかどうか
bool isClear = false;

//全ボディの削除
void DeleteAllBody(){
	if(world.GetBodyCount() > 0){	//ボディ数を調べる
		//ボディ削除ループ
		b2Body* b = world.GetBodyList(); 
		while(b != NULL){
			b2Body* nb = b->GetNext();
			world.DestroyBody(b);
			b = nb;
		}
	}
}

//ボディ生成
//x,y：中心座標
//w,h：幅,高さ(円の場合 w：半径)
//angle：角度
//isDynamic：動く物かどうか
//isBall：円かどうか
//density：密度
//restitution：反発力
//friction：摩擦力
b2Body* CreateBody(float x, float y, float h, float w, float angle,
	bool isDynamic, bool isBall, float density, float restitution, float friction)
{
	//ボディ定義
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;	//動的ボディ
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

	if(isDynamic){
		if(isBall)
			fixtureDef.shape = &dynamicBall;
		else
			fixtureDef.shape = &dynamicBox;
	}
	else{
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

//各ステージ初期化
void InitStage(int stage_num)
{
	//壁・床
	b2Body *floor[10];
	float x, y;//座標
	float w, h;//幅・高さ
	float density = 0.5;//密度
	float restitution = 0.5;//反発力
	float friction = 0.3f;//摩擦力

	switch(stage_num)
	{
	case 0:

		stage[stage_num].start.x = 50;
		stage[stage_num].start.y = 50;
		stage[stage_num].goal.x = 700;
		stage[stage_num].goal.y = 550;
		stage[stage_num].end.x = 450;
		stage[stage_num].end.y = 350;

		w = 150.0 * SCALE;
		h = 30.0 * SCALE;

		x = 150.0 * SCALE;
		y = 200.0 * SCALE;
		floor[0] = CreateBody(x, y, h, w, 0, false, false, density, restitution, friction);

		x = 700.0 * SCALE;
		y = 600.0 * SCALE;
		floor[1] = CreateBody(x, y, h, w, 0, false, false, density, restitution, friction);

		break;
	case 1:
		stage[stage_num].start.x = 100;
		stage[stage_num].start.y = 100;
		stage[stage_num].goal.x = 375;
		stage[stage_num].goal.y = 650;
		stage[stage_num].end.x = 450;
		stage[stage_num].end.y = 650;

		w = 100.0 * SCALE;
		h = 30.0 * SCALE;

		x = 150.0 * SCALE;
		y = 200.0 * SCALE;
		floor[0] = CreateBody(x, y, h, w, 0, false, false, density, restitution, friction);

		x = 600.0 * SCALE;
		y = 400.0 * SCALE;
		floor[1] = CreateBody(x, y, h, w, 0, false, false, density, restitution, friction);

		x = 300.0 * SCALE;
		y = 400.0 * SCALE;
		floor[2] = CreateBody(x, y, h, w, 0, false, false, density, restitution, friction);

		x = 450.0 * SCALE;
		y = 700.0 * SCALE;
		floor[3] = CreateBody(x, y, h, w, 0, false, false, density, restitution, friction);

		x = 200.0 * SCALE;
		y = 600.0 * SCALE;
		floor[4] = CreateBody(x, y, h, w, 0, false, false, density, restitution, friction);

		break;
	case 2:
		stage[stage_num].start.x = 50;
		stage[stage_num].start.y = 500;
		stage[stage_num].goal.x = 850;
		stage[stage_num].goal.y = 50;
		stage[stage_num].end.x = 300;
		stage[stage_num].end.y = 350;

		w = 100.0 * SCALE;
		h = 30.0 * SCALE;

		x = 800.0 * SCALE;
		y = 100.0 * SCALE;
		floor[0] = CreateBody(x, y, h, w, 0, false, false, density, restitution, friction);

		x = 650.0 * SCALE;
		y = 200.0 * SCALE;
		floor[1] = CreateBody(x, y, h, w, 0, false, false, density, restitution, friction);

		x = 500.0 * SCALE;
		y = 300.0 * SCALE;
		floor[2] = CreateBody(x, y, h, w, 0, false, false, density, restitution, friction);

		x = 350.0 * SCALE;
		y = 400.0 * SCALE;
		floor[3] = CreateBody(x, y, h, w, 0, false, false, density, restitution, friction);
		
		x = 200.0 * SCALE;
		y = 500.0 * SCALE;
		floor[4] = CreateBody(x, y, h, w, 0, false, false, density, restitution, friction);

		x = 100.0 * SCALE;
		y = 600.0 * SCALE;
		floor[5] = CreateBody(x, y, h, w, 0, false, false, density, restitution, friction);

		break;    
	}
}

void Init()
{
	//お絵かき関連
	//物体の点の数を初期化
	for(int i=0;i<MAX_OBJECT;i++)
		object[i].p_num=0;

	object[o_num].color = GetColor(GetRand(255), GetRand(255), GetRand(255));

	GetMousePoint(&x,&y);
	lx = -1000;
	ly = -1000;
}

void DrawObjects()
{

	for(b2Body *body = world.GetBodyList(); body!=NULL;body = body->GetNext() )
	{

		//形状別に処理をする
		for(b2Fixture *fixture = body->GetFixtureList();fixture!=NULL;fixture=fixture->GetNext())
		{
			//描画
			//矩形
			if(fixture->GetType() == b2Shape::e_polygon){
				if(body->GetType()== b2_dynamicBody)
					continue;
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
					vert[i].x /= SCALE;
					vert[i].y /= SCALE;
				}
				//四角形
				if(vertNum==4){
					DrawLine((float32)vert[0].x, vert[0].y, vert[1].x, vert[1].y, GetColor(0,255,255));
					DrawLine(vert[1].x, vert[1].y, vert[2].x, vert[2].y, GetColor(0,255,255));
					DrawLine(vert[2].x, vert[2].y, vert[3].x, vert[3].y, GetColor(0,255,255));
					DrawLine(vert[3].x, vert[3].y, vert[0].x, vert[0].y, GetColor(0,255,255));

					DrawLine(vert[2].x, vert[2].y, vert[0].x, vert[0].y, GetColor(0,255,255));
					DrawLine(vert[3].x, vert[3].y, vert[1].x, vert[1].y, GetColor(0,255,255));
				}
			}

			//円
			if( fixture->GetType() == b2Shape::e_circle ){
				b2CircleShape *poly = (b2CircleShape *)fixture->GetShape();
				//変換行列を取得する
				b2Transform transformB = body->GetTransform();
				b2Vec2 center = b2Mul(transformB,poly->m_p);

				float r = poly->m_radius;//半径;
				float x1,x2,y1,y2;
				float32 angle = body->GetAngle();

				x1 = r * cos(angle)+center.x;
				x2 = -r * cos(angle)+center.x;
				y1 = r * sin(angle)+center.y;
				y2 = -r * sin(angle)+center.y;

				DrawLine(x1/SCALE,y1/SCALE,x2/SCALE,y2/SCALE,GetColor(255,0,255));

				angle += M_PI/2;
				x1 = r * cos(angle)+center.x;
				x2 = -r * cos(angle)+center.x;
				y1 = r * sin(angle)+center.y;
				y2 = -r * sin(angle)+center.y;

				DrawLine(x1/SCALE,y1/SCALE,x2/SCALE,y2/SCALE,GetColor(255,0,255));

				DrawCircle(center.x/ SCALE, center.y/SCALE, r/SCALE, GetColor(255,0,255), FALSE);
			}
		}
	}
}

//お絵かき関連
void MouseDraw()
{
	int MouseState;
	MouseState = GetMouseInput();

	//頂点数が最大になった場合
	if( object[o_num].p_num == MAX_POS )
		MouseState=0;//なにも押されてない状態にする

	switch( MouseState )
	{
	case MOUSE_INPUT_LEFT://マウスの左ボタンまたはペンタブの通常描画
		GetMousePoint(&x,&y);//マウスポインタの座標取得
		// 円を描画
		if( hypot(lx - x, ly - y) > 5 || !isDraw )//描画感覚の調節
		{
			int p_num = object[o_num].p_num;
			//座標の保存
			object[o_num].pos[p_num].x = x;
			object[o_num].pos[p_num].y = y;

			object[o_num].p_num++;

			//1フレーム前の座標として保存
			lx = x;
			ly = y;

		}
		isDraw = true;//描画中
		break;

	default://入力がない場合

		if(object[o_num].p_num==1)//頂点一つでは物体を作らない
		{
			object[o_num].p_num=0;
			isDraw=false;

			break;
		}

		if(isDraw)//物体の描画を終了した場合
		{
			//ボディを一つ作る
			b2BodyDef bodyDef;
			bodyDef.type = b2_dynamicBody;
			object[o_num].DrawBody = world.CreateBody(&bodyDef);

			float bx, by;
			bx = (x + lx) / 2;
			by = (y + ly) / 2;
			float w;
			w = hypot( lx - x, ly - y );
			float angle;
			angle = atan2( float(ly - y), float(lx - x) );

			b2FixtureDef fixtureDef;

			float density = 1.0;//密度
			float restitution = 0.4;//反発力
			float friction = 0.7f;//摩擦力
			fixtureDef.density = density;
			fixtureDef.restitution = restitution;
			fixtureDef.friction = friction;

			//一つのbodyに対して複数のshapeを持たせる
			for( int i=1; i < object[o_num].p_num; i++ )
			{

				b2PolygonShape boxShape;

				//shapeの中心を現在のマウスポインタ座標と1フレーム前のマウスポインタの座標の中間にする
				bx = ( object[o_num].pos[i].x + object[o_num].pos[i-1].x )/2;
				by = ( object[o_num].pos[i].y + object[o_num].pos[i-1].y )/2;

				//幅は2点の距離にする
				w = hypot( object[o_num].pos[i-1].x - object[o_num].pos[i].x ,  object[o_num].pos[i-1].y - object[o_num].pos[i].y ) * SCALE ;

				angle = atan2( (float)(object[o_num].pos[i-1].y - object[o_num].pos[i].y) ,(float)(object[o_num].pos[i-1].x - object[o_num].pos[i].x) );

				boxShape.SetAsBox(w, PEN_W * SCALE, b2Vec2(bx * SCALE,by * SCALE), angle);

				fixtureDef.shape = &boxShape;

				b2Fixture *fixture;

				fixture = object[o_num].DrawBody->CreateFixture(&fixtureDef);

			}

			o_num++;
			object[o_num].color = GetColor(GetRand(255),GetRand(255),GetRand(255));
		}
		isDraw = false;
		break;
	}

	//お絵かき中のマウスポインタを描画
	if(isDraw)
	{
		for(int j =0; j < object[o_num].p_num; j++)
		{
			int dx, dy;
			int color;
			dx = object[o_num].pos[j].x;
			dy = object[o_num].pos[j].y;
			color = object[o_num].color;
			DrawCircle( dx , dy, PEN_W , color , true ) ;
		}
	}

	//物体の描画
	for(int i = 0; i < o_num; i++)
	{
		int color = object[i].color;
		for(b2Fixture *fixture = object[i].DrawBody->GetFixtureList(); fixture!=NULL; fixture=fixture->GetNext())
		{
			//描画
			//矩形
			if(fixture->GetType() == b2Shape::e_polygon){
				b2PolygonShape *poly = (b2PolygonShape*)fixture->GetShape();
				b2Vec2 vert[b2_maxPolygonVertices];
				int vertNum = poly->GetVertexCount();
				//変換行列を取得する
				b2Transform transformB = object[i].DrawBody->GetTransform();
				//矩形の制御点を取得する

				for(int i=0; i<vertNum; ++i){
					vert[i] = poly->m_vertices[i];
					//変換行列で制御点を変換する
					vert[i] = b2Mul(transformB, vert[i]);
					//スケール変換
					vert[i].x /= SCALE;
					vert[i].y /= SCALE;
				}

				//四角形
				if(vertNum==4){
					DrawLine((float32)vert[0].x, vert[0].y, vert[1].x, vert[1].y, color);
					DrawLine(vert[1].x, vert[1].y, vert[2].x, vert[2].y, color);
					DrawLine(vert[2].x, vert[2].y, vert[3].x, vert[3].y, color);
					DrawLine(vert[3].x, vert[3].y, vert[0].x, vert[0].y, color);

					DrawLine(vert[2].x, vert[2].y, vert[0].x, vert[0].y, color);
					DrawLine(vert[3].x, vert[3].y, vert[1].x, vert[1].y, color);
				}
			}
		}
	}
}

//ゲームクリア判定
void checkClear()
{
	int bodycnt = 0;
	for(b2Body *body = ball; body!=NULL ;body = body->GetNext() )
	{
		//printfDx( "%d %p\n", bodycnt, body );
		//形状別に処理をする
		for(b2Fixture *fixture = body->GetFixtureList();fixture!=NULL;fixture=fixture->GetNext())
		{
			//円形なら処理を行う
			if( fixture->GetType() != b2Shape::e_circle )
				continue;
			b2CircleShape *poly = (b2CircleShape *)fixture->GetShape();

			//変換行列を取得する
			b2Transform transformB = body->GetTransform();
			b2Vec2 center = b2Mul(transformB,poly->m_p);

			float jx,jy;
			jx = stage[stage_num].goal.x - center.x / SCALE;
			jy = stage[stage_num].goal.y - center.y / SCALE;

			float ex,ey;
			ex = stage[stage_num].end.x - center.x / SCALE;
			ey = stage[stage_num].end.y - center.y / SCALE;

			if( hypot(jx, jy) < GOAL_SIZE + BALL_SIZE )//ゴール
			{
				DeleteAllBody();//全Bodyの消去
				isBall = false;
				stage_num++;//ステージを一つ進める
				if(stage_num == STAGE_NUM)
				{
					DrawString(CENTER_X-100, CENTER_Y, "Game Clear", GetColor(255, 255, 255));
				}
				InitStage(stage_num);

				o_num = 0;

				for(int i=0;i<MAX_OBJECT;i++)
					object[i].p_num=0;
				GetMousePoint(&x,&y);
				lx = -1000;
				ly = -1000;

				isDraw = false;
				return;
			}
			else if( hypot(ex, ey) < GOAL_SIZE + BALL_SIZE )//ゴール
			{
				DeleteAllBody();//全Bodyの消去
				isBall = false;
				DrawString(CENTER_X-100, CENTER_Y, "Game Over", GetColor(255, 255, 255));
				isDraw = false;
				stage_num=STAGE_NUM;
				return;
			}
		}
	}
}

//ゲームクリア判定
void CheckClearStage()
{
	for(b2Body *body = ball; body!=NULL ;body = body->GetNext() )
	{
		//形状別に処理をする
		for(b2Fixture *fixture = body->GetFixtureList();fixture!=NULL;fixture=fixture->GetNext())
		{
			//円形なら処理を行う
			if( fixture->GetType() != b2Shape::e_circle )
				continue;
			b2CircleShape *poly = (b2CircleShape *)fixture->GetShape();

			//変換行列を取得する
			b2Transform transformB = body->GetTransform();
			b2Vec2 center = b2Mul(transformB,poly->m_p);

			float jx,jy;
			jx = stage[stage_num].goal.x - center.x / SCALE;
			jy = stage[stage_num].goal.y - center.y / SCALE;

			float ex,ey;
			ex = stage[stage_num].end.x - center.x / SCALE;
			ey = stage[stage_num].end.y - center.y / SCALE;

			if( hypot(jx, jy) < GOAL_SIZE + BALL_SIZE )//ゴール
			{
				//isClear = true;

				DeleteAllBody();//全Bodyの消去
				isBall = false;
				stage_num++;//ステージを一つ進める
				InitStage(stage_num);

				o_num = 0;

				for(int i=0;i<MAX_OBJECT;i++)
					object[i].p_num=0;
				GetMousePoint(&x,&y);
				lx = -1000;
				ly = -1000;

				isDraw = false;
				if(stage_num == STAGE_NUM)
					DrawString(CENTER_X-100, CENTER_Y, "Game Clear", GetColor(255, 255, 255));
				return;
			}
			else if( hypot(ex, ey) < GOAL_SIZE + BALL_SIZE )//ゴール
			{
				//isClear = true;

				DeleteAllBody();//全Bodyの消去
				isBall = false;
				isDraw = false;
				DrawString(CENTER_X-100, CENTER_Y, "Game Over", GetColor(255, 255, 255));
				stage_num=STAGE_NUM;
				return;
			}
		}
	}
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )

{
	ChangeWindowMode(true);
	SetGraphMode(960, 720, 32); 
	if( DxLib_Init() == -1 )//ＤＸライブラリ初期化処理
		return -1;//エラーが起きたら直ちに終了
	//初期化
	Init();

	//描画先を裏画面に設定
	SetDrawScreen( DX_SCREEN_BACK );

	//物理計算用変数
	//更新速度
	float32 timeStep = 1.0f / 60.0f;
	//反復数
	int32 velocityIterations = 6;
	int32 positionIterations = 2;

	bool HitFlag_SPACE = false;//キー操作用
	bool HitFlag_Z = false;//キー操作用
	bool isClear = false;

	int count=0;

	InitStage(stage_num);//ステージの初期化

	DrawString(CENTER_X-100,CENTER_Y,"Press any key",GetColor(255,255,255));//何かキーを押して開始
	ScreenFlip();
	WaitKey();

	//Escキーで終了
	while(!ProcessMessage() && !CheckHitKey(KEY_INPUT_ESCAPE) && stage_num<STAGE_NUM )
	{
		//画面を初期化
		ClearDrawScreen();

		//全ての物体を消す
		if( CheckHitKey(KEY_INPUT_Z) && !HitFlag_Z ){
			DeleteAllBody();//全Bodyの消去
			InitStage(stage_num);

			//お絵かき用変数の初期化
			o_num = 0;

			for(int i=0;i<MAX_OBJECT;i++)
				object[i].p_num=0;

			GetMousePoint(&x,&y);

			lx = -1000;
			ly = -1000;

			//各フラグを設定
			isDraw = false;
			HitFlag_Z = true;
			isBall = false;
			continue;

		}
		if( !CheckHitKey(KEY_INPUT_Z) )
			HitFlag_Z = false;

		if( CheckHitKey(KEY_INPUT_SPACE) && !HitFlag_SPACE ){

			HitFlag_SPACE = true;
			isBall = true;//ボールが存在するかどうか

			float density = 0.5;//密度
			float restitution = 0.5;//反発力
			float friction = 0.3f;//摩擦力

			float sx,sy;
			sx = stage[stage_num].start.x * SCALE;
			sy = stage[stage_num].start.y * SCALE;

			count++;

			float w = BALL_SIZE * SCALE;

			ball = CreateBody( sx, sy, 0, w, 0, true, true, density, restitution, friction);
		}
		if( !CheckHitKey(KEY_INPUT_SPACE) )
			HitFlag_SPACE = false;

		//物体生成
		MouseDraw();

		//物理計算
		world.Step(timeStep, velocityIterations, positionIterations);
		world.ClearForces();

		//ゴールの描画
		DrawCircle(stage[stage_num].goal.x, stage[stage_num].goal.y, GOAL_SIZE, GetColor(255,255,255), true);

		//これに当たればゲームオーバー
		DrawCircle(stage[stage_num].end.x, stage[stage_num].end.y, GOAL_SIZE, GetColor(255,0,0), true);

		//物体の描画
		DrawObjects();

		//ゲームクリア判定
		if(isBall )//ボールが存在するなら
			checkClear();

		//裏画面の内容を表画面に反映
		ScreenFlip();
	}

	//裏画面の内容を表画面に反映
	ScreenFlip();
	
	WaitKey();
	DxLib_End();//ＤＸライブラリ使用の終了処理

	return 0;//ソフトの終了 
}