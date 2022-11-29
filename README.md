Syatem Programing Professor 김경훈

HW01 
-------------------------------------
ls명령어 시간순, FileSize순 Sorting

Cache
-------------------------------------
Set Association Cache Simulator 구현.

args
-a = SetSize
-s = CacheSize
-b = BlockSize
-f = Filename

Trc.File
Address = 32bit 주소를 16진수로 줌
Command = char ( Write or Read )
만약 Command == Write 이면 Int형 Data

Logic
1. args로 Cache 구조체 구성
   16진수 -> 2진수 변환 후, Tag_bits, Index_bits, Block_offsets, Byte_offsets(2bits)로 나누어 동적할당
   
2. inputFile로 부터 Address, Command, Data받음
   Address -> 2진수 변환 후 Bitd위치에 맞추어 int형 (index, tag, block, byte) 변환

3. 2에서 얻은 Paramater로 Cache 접근
  Hit -> Command 수행
  Miss -> Set이 Full인지 확인 -> FULL이면 MEM_ADD 후 Cache_Update
                           -> Full이 아니면 Cache_Update
  3-1 Set Full 처리
      원형 큐 응용 -> 일단한번 Cache Set에 데이터가 들어오고나면, 무조건 1개 이상 들어가있음. Ex) Association = 2 이고, 한 index에 1번이상 들어갔다면 무조건 1이상.
                -> Latest_index, Fastest_Index를 Cache의 Index 주소 개수만큼 동적할당 / Latest = 0, Fastest = -1로 초기화
                -> 한번 접근 Lastest = 추가해야 할 Set_index // Fastest =  0
