ROSEmulator - CMAI2100Dj 연동

빌드
- ROSEmulator.sln을 Visual Studio에서 열고 Win32로 빌드합니다.
- 원본 프로젝트 설정은 v141, Windows SDK 10.0.19041.0, MFC입니다.
- 결과는 bin\Debug 또는 bin\Release 아래에 생성됩니다.

장비 PC와 에뮬레이터 PC를 분리해서 연동
1. CMAI2100Dj의 ROS 사용 옵션을 켭니다. Run\System\RosData.ini의
   [DISPATCHER] HANDLER_IP에는 실제 장비 PC IPv4 주소를 지정합니다.
2. 에뮬레이터의 ROS local IP에는 에뮬레이터 PC의 실제 IPv4 주소를,
   Handler IP에는 장비 PC의 주소를 입력합니다. UDP port는 8020입니다.
   화면 기본값은 첨부 INI의 192.168.100.130 (ROS), 192.168.100.10 (Handler)입니다.
3. 장비 프로그램이 실행된 상태에서 에뮬레이터의 Start를 누릅니다.
   Start는 CONNECT REQUEST를 보냅니다. Handler가 나중에 실행됐으면
   Connect request 버튼을 눌러 다시 등록합니다.
   Handler가 시작할 때 먼저 연결하려면 RosData.ini의 [MDJ_LIST] 00에 ROS IP를
   지정하고 에뮬레이터를 먼저 실행합니다. 판정 요청이 다른 ROS 서버로 분산되지
   않도록 시험 환경에서는 시험 대상 ROS IP만 등록합니다.
4. 양쪽 PC에서 상대 PC의 UDP 8020 패킷을 받을 수 있어야 합니다.

같은 Windows PC에서 연동
1. 시험용 CMAI2100Dj Dispatcher.cpp의 UDP_DISPATCHER_HPORT만 8021로
   변경하고 재빌드합니다. UDP_DISPATCHER_LPORT는 8020을 사용합니다.
2. 시험용 RosData.ini의 HANDLER_IP는 127.0.0.1로 지정합니다.
   MDJ_LIST를 사전 등록하는 경우 00에도 127.0.0.1을 지정합니다.
3. 에뮬레이터에서 두 IP를 모두 127.0.0.1로 입력하고 UDP port를 8021로
   변경합니다. 나머지 연결 순서는 위와 같습니다.

판정 확인
- 처음에는 Always G를 선택하고 ROS 판정 요청이 발생하는 검사를 실행합니다.
- 에뮬레이터 화면의 RX JUDGE REQUEST와 TX JUDGE REPLY를 확인합니다.
- 장비의 Run\LOG\Dispatcher 로그에서 [Get_JudgeReply] Judge Result를
  확인합니다. 요청과 응답의 Port/Tray/Module 번호가 같아야 합니다.
- 요청의 이미지 개수는 응답에서 제외됩니다. 응답 끝은 G, N 또는 R입니다.
- Delay min/max는 ms 단위입니다. 정상 판정 시험에서는 장비 판정 타임아웃
  이내로 설정합니다. No reply는 의도적으로 판정 응답을 생략합니다.
