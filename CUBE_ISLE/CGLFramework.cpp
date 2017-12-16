#include "CGLFramework.h"

CGLFramework::CGLFramework()
{
}

CGLFramework::~CGLFramework()
{
}

void CGLFramework::Initialize(int argc, char * argv[], int width, int height, int x, int y, const char * caption, int DsplayMove)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(DsplayMove);	// 디스플레이 모드 설정
	glutInitWindowSize(width, height);		// 윈도우 크기
	glutInitWindowPosition(x, y);	// 윈도우 위치지정
	glutCreateWindow(caption);

	// 사운드
	m_soundmgr.AddSound("BGM", "Sound/RefunctGameOst.mp3", SoundType::Stream);
	PushPlayQueue("BGM", Vector3(0, 0, 0));
//	m_soundmgr.AddSound("Click", "Sound/Equip.wav", 700, 1000); // cube 크기 기준으로
}

void CGLFramework::Run()
{
	glutMainLoop();
}

void CGLFramework::DrawScene()
{
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glLoadIdentity();
	
	Render();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	glutSwapBuffers();
}

void CGLFramework::Render()
{
	cubemap.draw();
	//m_zakka.Draw();
	//m_key.Draw();
}

void CGLFramework::Reshape(int w, int h)
{
	// 뷰포트 변환 설정: 출력 화면 결정
	glViewport(0, 0, w, h);

	//// 투영 행렬 스택 재설정
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();

	//// 클리핑 공간 설정 (원근 투영)
	//gluPerspective(60.0f, (float)w / (float)h, 1.0, 1000.0);
	//glTranslatef(0.0, 0.0, -610.f); // 투영 공간을 화면 안쪽으로 이동하여 시야를 확보한다.

	//								// 모델 뷰 행렬 스택 재설정
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();

	//// 관측 변환: 카메라의 위치 설정
	//gluLookAt(0.0, 0.0, 0.0,	// 카메라의 위치
	//	0.0, 0.0, 1.0,			// 카메라가 바라보는 점
	//	0.0, 1.0, 0.0);			// 카메라의 기울기

	camera.Initialize({ 0.f, 0.f, 0.f }, 610.f, 1.f, 1000.f, 60.f);

	m_key.Init();
}

void CGLFramework::Keyboard(unsigned char key, int x, int y)
{
	if (key == 'p' || key == 'P') {
		cubemap.Init();
	}
	else if (key == 'g' || key == 'G') {
		cubemap.is_draw_aabb = !cubemap.is_draw_aabb;
	}
	else if (key == 'q' || key == 'Q') { // 종료
		exit(0);
	}
	else if (key == 'i') { // 초기화
		is_fpv = false;
		camera.Initialize();
	}
	else if (key == '-') {
		camera.zoom(-10);
	}
	else if (key == '+') {
		camera.zoom(10);
	}
	else if (key == '0') {	
	}
	else if (key == '.') {
	}
	else if (key == 't') {
		camera.Rotate(0, radian(10));
	}
	else if (key == 'T') {
		camera.Rotate(0, radian(-10));
	}
	else if (key == 'y') {
		camera.Rotate(radian(10), 0);
	}
	else if (key == 'Y') {
		camera.Rotate(radian(-10), 0);
	}
	else if (key == ' ') { // 제어점 추가
	
		//	PushPlayQueue("Click", cube.cardinal.current_pts);
	}
	else if (key == 'z') { // 제어점 삭제
	}
	else if (key == 'x') { // 제어점 초기화
	}
	else if (key == '1') {
		is_fpv = true;
	}
	else if (key == '3') {
		is_fpv = false;
		camera.Initialize();
	}

	glutPostRedisplay();					// 화면 재출력
}

void CGLFramework::SpecialKeyboard(int key, int x, int y)
{
	if (key == GLUT_KEY_LEFT) {
	}
	else if (key == GLUT_KEY_RIGHT) {
	}
	else if (key == GLUT_KEY_UP) {
	}
	else if (key == GLUT_KEY_DOWN) {
	}
}

void CGLFramework::Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		drag = true;
		mStart = { (float)x, (float)y };
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		if (!drag) return;
		drag = false;
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
	{

	}
}

void CGLFramework::Motion(int x, int y)
{
	if (is_fpv) return;
	if (drag) {
		mEnd = { (float)x,(float)y };
		/*cube.yRot = (cube.yRot + (int)(mEnd.x - mStart.x)) % 360;
		cube.xRot = (cube.xRot + (int)(mEnd.y - mStart.y)) % 360;*/
		int yRot = ((int)(mEnd.x - mStart.x)) % 360;
		yRot = -yRot;
		int xRot = ((int)(mEnd.y - mStart.y)) % 360;
		camera.Rotate(radian(yRot), radian(xRot));
		mStart = { (float)x,(float)y };
	}
}

void CGLFramework::Timer(int value)
{
	// look up pos
	FMOD_System_Instance()->update(0.016, camera.m_at - camera.m_pos, camera.m_up, camera.m_pos);
	if (PlayQueueSize() > 0) // 이벤트 큐 패턴
	{
		auto info = PopPlayQueue();
		m_soundmgr.Play(info.first, info.second);
	}

	m_key.Update();

	glutTimerFunc(m_fps, fnTimer, 1);
	glutPostRedisplay();
}
