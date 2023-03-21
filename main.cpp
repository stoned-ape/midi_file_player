#ifdef __clang__
#pragma clang diagnostic ignored "-Wnonportable-system-include-path"
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#pragma clang diagnostic ignored "-Wc++98-compat"
#pragma clang diagnostic ignored "-Wc++98-compat-pedantic"
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#pragma clang diagnostic ignored "-Wnested-anon-types"
#pragma clang diagnostic ignored "-Wfour-char-constants"
#pragma clang diagnostic ignored "-Wunreachable-code"
#pragma clang diagnostic ignored "-Wdouble-promotion"
#pragma clang diagnostic ignored "-Wunused-variable"
#else
#pragma warning(disable:4201)
#pragma warning(disable:4820)
#pragma warning(disable:4668)
#pragma warning(disable:4702)
#pragma warning(disable:5045)
#pragma warning(disable:4189)
#pragma warning(disable:5264)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)
#endif
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <dsound.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>
#include <math.h>
#include <time.h>
#include <sys/timeb.h>

#include <vector>
#include <algorithm>

//C:\Program Files (x86)\Windows Kits\10\Include\10.0.22621.0\um\dsound.h

#pragma comment(lib,"dsound.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"user32.lib") 
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"opengl32.lib")



#define PRINT(val,type) printf("%s:\t" type "\n",#val,val)

const float pi=3.141592653589793f;


double itime(){
    struct timeb now;
    ftime(&now);
    return (double)(now.time%(60*60*24))+now.millitm/1e3;
}



void ds_check(HRESULT hr,const char *code,int line,const char *file){
	switch(hr){
	#define CASE(val) case val:{\
		fprintf(stderr,"%s = %s line %d, file %s\n",#val,code,line,file);\
		exit(1);\
	}break
	case DS_OK: break;
	CASE(DS_NO_VIRTUALIZATION);
	CASE(DSERR_ALLOCATED);
	CASE(DSERR_CONTROLUNAVAIL);
	CASE(DSERR_INVALIDPARAM);
	CASE(DSERR_INVALIDCALL);
	CASE(DSERR_GENERIC);
	CASE(DSERR_PRIOLEVELNEEDED);
	CASE(DSERR_OUTOFMEMORY);
	CASE(DSERR_BADFORMAT);
	CASE(DSERR_UNSUPPORTED);
	CASE(DSERR_NODRIVER);
	CASE(DSERR_ALREADYINITIALIZED);
	CASE(DSERR_NOAGGREGATION);
	CASE(DSERR_BUFFERLOST);
	CASE(DSERR_OTHERAPPHASPRIO);
	CASE(DSERR_UNINITIALIZED);
	CASE(DSERR_NOINTERFACE);
	CASE(DSERR_ACCESSDENIED);
	CASE(DSERR_BUFFERTOOSMALL);
	CASE(DSERR_DS8_REQUIRED);
	CASE(DSERR_SENDLOOP);
	CASE(DSERR_BADSENDBUFFERGUID);
	CASE(DSERR_OBJECTNOTFOUND);
	CASE(DSERR_FXUNAVAILABLE);
	default: puts("unknown error");
	#undef CASE
	}
}

#define DS_CHECK(call) ds_check(call,#call,__LINE__,__FILE__)

LRESULT CALLBACK WndProc(HWND hwnd,UINT message,WPARAM wparam,LPARAM lparam){
	switch(message){
	case WM_CLOSE: 
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd,message,wparam,lparam);
}

uint32_t vlq2int(uint8_t *v,uint32_t *len){
	typedef struct{
		uint8_t val:7;
		uint8_t bit:1;
	}vlq_byte;
	static_assert(sizeof(vlq_byte)==1,"");
	uint32_t res=0,pow=1;
	uint32_t j=4;
	for(uint32_t i=0;i<4;i++){
		vlq_byte vb=*(vlq_byte*)(v+i);
		if(!vb.bit){
			j=i;
			if(len) *len=i+1;
			break;
		}
		pow<<=7;
	}
	assert(j!=4);
	assert(v[j]<128);
	for(uint32_t i=0;i<=j;i++){
		vlq_byte vb=*(vlq_byte*)&v[i];
		res+=pow*vb.val;
		pow>>=7;
	}
	return res;
}

void test_vlq2int(){
	puts(__func__);
	uint8_t v0[]={0x00};
	uint8_t v1[]={0x40};
	uint8_t v2[]={0x7F};
	uint8_t v3[]={0x81,0x00};
	uint8_t v4[]={0xC0,0x00};
	uint8_t v5[]={0xFF,0x7F};
	uint8_t v6[]={0x81,0x80,0x00};
	uint8_t v7[]={0xC0,0x80,0x00};
	uint8_t v8[]={0xFF,0xFF,0x7F};
	uint8_t v9[]={0x81,0x80,0x80,0x00};
	uint8_t v10[]={0xC0,0x80,0x80,0x00};
	uint8_t v11[]={0xFF,0xFF,0xFF,0x7F};
	
	uint8_t *vlqs[]={v0,v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11};
	uint32_t lens[]={
		sizeof(v0),sizeof(v1),sizeof(v2),
		sizeof(v3),sizeof(v4),sizeof(v5),
		sizeof(v6),sizeof(v7),sizeof(v8),
		sizeof(v9),sizeof(v10),sizeof(v11),
	};
	uint32_t ints[]={
		0x00000000,0x00000040,0x0000007F,0x00000080,
		0x00002000,0x00003FFF,0x00004000,0x00100000,
		0x001FFFFF,0x00200000,0x08000000,0x0FFFFFFF
	};
	uint32_t len;

	assert(vlq2int(vlqs[0],&len)==ints[0]);assert(len==lens[0]);
	assert(vlq2int(vlqs[1],&len)==ints[1]);assert(len==lens[1]);
	assert(vlq2int(vlqs[2],&len)==ints[2]);assert(len==lens[2]);
	assert(vlq2int(vlqs[3],&len)==ints[3]);assert(len==lens[3]);
	assert(vlq2int(vlqs[4],&len)==ints[4]);assert(len==lens[4]);
	assert(vlq2int(vlqs[5],&len)==ints[5]);assert(len==lens[5]);
	assert(vlq2int(vlqs[6],&len)==ints[6]);assert(len==lens[6]);
	assert(vlq2int(vlqs[7],&len)==ints[7]);assert(len==lens[7]);
	assert(vlq2int(vlqs[8],&len)==ints[8]);assert(len==lens[8]);
	assert(vlq2int(vlqs[9],&len)==ints[9]);assert(len==lens[9]);
	assert(vlq2int(vlqs[10],&len)==ints[10]);assert(len==lens[10]);
	assert(vlq2int(vlqs[11],&len)==ints[11]);assert(len==lens[11]);
}

#if defined(__clang__) || defined(__GNUC__)
#define PACKED __attribute__((packed)) 
#else
#define PACKED __pragma(pack(push,1))   
#endif

struct PACKED midi_file_header{
	uint32_t chunk_type;
	uint32_t length;
	uint16_t format;
	uint16_t ntracks;
	int16_t division;
};

static_assert(sizeof(midi_file_header)==14,"");

typedef enum:uint8_t{
	NOTE_OFF=0x8,
	NOTE_ON=0x9,
	AFTER_TOUCH1=0xa,
	CTRL_CHG=0xb,
	PROG_CHG=0xc,
	AFTER_TOUCH2=0xd,
	PICH_CHG=0xe,
	SYS_MSG=0xf,
}midi_event_type;

struct PACKED midi_event{
	union{
		uint8_t status;
		struct{
			uint8_t chan:4;
			uint8_t type:4;
		};
	};
	uint8_t key:7;
	uint8_t zero1:1;
	uint8_t velocity:7;
	uint8_t zero2:1;
};

struct PACKED midi_track_header{
	uint32_t chunk_type;
	uint32_t length;
};

struct note_message{
	int32_t track;
	uint32_t time;
	uint8_t key;
	uint8_t volume;
};


int WINAPI WinMain(	[[maybe_unused]]HINSTANCE hi,
					[[maybe_unused]]HINSTANCE hpi,
					[[maybe_unused]]char *args,
					[[maybe_unused]]int winshow){
	AttachConsole((DWORD)-1);
	freopen("CONIN$","r",stdin);
	freopen("CONOUT$","w",stdout);
	freopen("CONOUT$","w",stderr);
	puts("");

	// test pipes
	// puts("stdout");
	// fputs("stderr\n",stderr);
	
	test_vlq2int();	

	puts(args);
	// const char *fname="minuet_in_g.mid";
	// const char *fname="cannon_in_c.mid";
	// const char *fname="fur_elise.mid";
	// const char *fname="gnossienne.mid";
	// const char *fname="spring.mid";
	const char *fname=args;
	HANDLE file=CreateFile(fname,GENERIC_READ,0,0,OPEN_EXISTING,0,0);
	assert(file!=INVALID_HANDLE_VALUE);

	uint64_t file_size;
	static_assert(sizeof(_LARGE_INTEGER)==8,"");
	assert(GetFileSizeEx(file,(_LARGE_INTEGER*)&file_size));

	uint8_t *raw_file=(uint8_t*)malloc(file_size);

	unsigned long read_size;
	assert(ReadFile(file,raw_file,(uint32_t)file_size,&read_size,0));
	assert(read_size==file_size);

	CloseHandle(file);
	file=NULL;

	
	auto *head=(midi_file_header*)raw_file;
	assert(head->chunk_type==htonl('MThd'));
	assert(htonl(head->length)==6);
	PRINT(htons(head->format),"%d");
	assert(htons(head->format)<3);
	PRINT(htons(head->ntracks),"%d");
	assert(htons(head->ntracks)>0);
	assert((int16_t)htons((uint16_t)head->division)>0);

	uint32_t usec_per_qar=0xffffffff;
	std::vector<note_message> messages;
	uint32_t max_timer=0;
	uint8_t *track_ptr=raw_file+sizeof(midi_file_header);
	for(int i=0;i<htons(head->ntracks);i++){
		auto *track_head=(midi_track_header*)track_ptr;
		assert(track_head->chunk_type==htonl('MTrk'));
		uint32_t track_size=htonl(track_head->length);
		PRINT(track_size,"%u");
		assert(track_size<file_size);

		uint8_t running_status=0;
		bool end_of_track=false;
		uint8_t *event_ptr=track_ptr+sizeof(midi_track_header);
		uint8_t *track_end=event_ptr+track_size;
		uint32_t timer=0;
		while(event_ptr<track_end){
			uint32_t len;
			uint32_t delta_time=vlq2int((uint8_t*)event_ptr,&len);
			// PRINT(delta_time,"%u");
			timer+=delta_time;
			event_ptr+=len;
			uint8_t status=*event_ptr;
			// PRINT(status,"%d");
			if(running_status<=127) assert(status>127);
			switch(status){
				//meta-event
			case 0xff:{
				running_status=0;
				// puts("meta-event");
				event_ptr++;
				uint8_t type=*event_ptr;
				event_ptr++;
				// PRINT(type,"0x%x");
				assert(type<128);
				if(type<=0x7 || type==0x7f){
					uint32_t length=vlq2int(event_ptr,&len);
					event_ptr+=len;
					puts((char*)event_ptr);
					// assert(memcmp(event_ptr,"Minuet In G",11)==0);
					event_ptr+=length;
				}else{
					switch(type){
					case 0x20:
						// assert(0==1);
						event_ptr+=2;
						break;
					case 0x2f:
						// assert(0==1);
						assert(*event_ptr==0x0);
						event_ptr+=1;
						end_of_track=true;
						break;
					case 0x51:{ //tempo
						// assert(0==1);
						assert(*event_ptr==0x3);
						static auto get24=[](void *v)->uint32_t{
							struct _get24{
								uint32_t lo:8,hi:24;
							};
							static_assert(sizeof(_get24)==4,"");
							auto s=*(_get24*)v;
							return htonl(s.hi)>>8;
						};
						uint32_t x=0xddccbbaa;
						assert(get24(&x)==0xbbccdd);

						auto y=get24(event_ptr);
						usec_per_qar=min(usec_per_qar,y);
						
						event_ptr+=4;
					}	break;
					case 0x54:
						assert(0==1);
						event_ptr+=6;
						break;
					case 0x58: //time signature
						assert(*event_ptr==0x4);
						event_ptr+=5;
						break;
					case 0x59: //key signature
						assert(*event_ptr==0x2);
						event_ptr+=3;
						break;
					case 0x21:
						event_ptr+=2;
						break;
					default:
						assert(1==0);
					}
				}
				// if(j>20) return 0;
				// assert(0xff==0);
				break;
			}
				//SYSTEM COMMON MESSAGES 
			case 0xF1: assert(1==0);break;
			case 0xF2: assert(1==0);break;
			case 0xF3: assert(1==0);break;
			case 0xF4: assert(1==0);break;
			case 0xF5: assert(1==0);break;
			case 0xF6: assert(1==0);break;
			// case 0xF7: assert(1==0);
				//SYSTEM REAL TIME MESSAGES
			case 0xF8: assert(1==0);break;
			case 0xF9: assert(1==0);break;
			case 0xFA: assert(1==0);break;
			case 0xFB: assert(1==0);break;
			case 0xFC: assert(1==0);break;
			case 0xFD: assert(1==0);break;
			case 0xFE: assert(1==0);break;
			// case 0xFF: assert(1==0);
				//sysex events
			case 0xf0:
				assert(0xf0==0);
				break;
			case 0xf7:
				assert(0xf7==0);
				break;
				//midi event
			default:
				midi_event me=*(midi_event*)event_ptr;
				if(status<128){
					event_ptr--;
					me=*(midi_event*)(event_ptr);
					me.status=running_status;
				}else{
					me=*(midi_event*)event_ptr;
					running_status=status;
				}
				switch(me.type){
					//3 bytes
				case NOTE_ON:
					if(me.velocity!=0){
						messages.push_back(note_message{i,(uint32_t)timer,me.key,me.velocity});
						// puts("NOTE_ON");
						// printf("\t");PRINT(delta_time,"%u");
						// printf("\t");PRINT(event_ptr[0],"%u");
						// printf("\t");PRINT(me.key,"%u");
						// printf("\t");PRINT(me.velocity,"%u");
						assert(me.zero1==0 && me.zero2==0);
						event_ptr+=sizeof(midi_event);
						break;
					}
				// puts("fallthrough");
				[[fallthrough]]; 
				case NOTE_OFF:
					messages.push_back(note_message{i,(uint32_t)timer,me.key,me.velocity});
					// puts("NOTE_OFF");
					// printf("\t");PRINT(delta_time,"%u");
					// printf("\t");PRINT(event_ptr[0],"%u");
					// printf("\t");PRINT(me.key,"%u");
					// printf("\t");PRINT(me.velocity,"%u");
					assert(me.zero1==0 && me.zero2==0);
					event_ptr+=sizeof(midi_event);
					break;
				case AFTER_TOUCH1:
					// puts("AFTER_TOUCH1");
					event_ptr+=sizeof(midi_event);
					break;
				case CTRL_CHG:
					// puts("CTRL_CHG");
					event_ptr+=sizeof(midi_event);
					break;
				case PICH_CHG:
					// puts("PICH_CHG");
					event_ptr+=sizeof(midi_event);
					break;
					//2 bytes
				case PROG_CHG:
				case AFTER_TOUCH2:
					event_ptr+=2;
					break;
					//varible number of bytes
				case SYS_MSG:
					// puts("SYS_MSG");
					assert(1==0);
					event_ptr+=2;
					break;
				default:
					assert(0==1);
					event_ptr+=2;
					break;
				}
			}
			if(end_of_track) break;
		}
		PRINT(timer,"%u");
		if(timer>max_timer) max_timer=timer;
		track_ptr+=track_size+sizeof(midi_track_header);
	}


	PRINT(max_timer,"%u");
	PRINT(messages.size(),"%zu");

	assert(messages.size()>1);

	std::sort(messages.begin(),messages.end(),[](auto a,auto b)->bool{
		return a.time<b.time;
	});


	int middle_c_idx=60;
	float freqs[128];
	freqs[middle_c_idx]=256.0f;
	float twelth_root_2=1.0594630943592953f;
	for(int i=middle_c_idx+1;i<128;i++){
		freqs[i]=freqs[i-1]*twelth_root_2;
	}
	for(int i=middle_c_idx-1;i>=0;i--){
		freqs[i]=freqs[i+1]/twelth_root_2;
	}

	float key_volumes[128]={};



	// return 0;
	
	WNDCLASS wndclass={};
	wndclass.style        =CS_OWNDC;
	wndclass.lpfnWndProc  =WndProc;
	wndclass.hInstance    =hi;
	wndclass.hCursor      =LoadCursor(NULL,IDC_ARROW);
	wndclass.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszClassName="window_class";

	

	assert(RegisterClass(&wndclass));
	HWND hwnd=CreateWindow(
		"window_class",
		"midi player ._____.",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		300,
		30,
		NULL,
		NULL,
		hi,
		NULL
	);
	assert(hwnd);
	ShowWindow(hwnd,winshow);


	uint32_t num_tic=max_timer;
	uint32_t smp_per_sec=44100;
	uint32_t tic_per_qar=htons((uint16_t)head->division);
	uint32_t tic_per_sec=(1'000'000*tic_per_qar)/usec_per_qar;
	uint32_t smp_per_tic=smp_per_sec/tic_per_sec; 
	uint32_t num_sec=num_tic/tic_per_sec;
	uint16_t nchans=2;
	uint16_t bits_per_mono_samp=16;
	uint64_t data_size=(nchans*bits_per_mono_samp/8)*smp_per_tic*num_tic;

	PRINT(tic_per_qar,"%d");
	PRINT(usec_per_qar,"%d");
	PRINT(smp_per_sec,"%u");
	PRINT(tic_per_sec,"%u");
	PRINT(smp_per_tic,"%u");


	struct IDirectSound8 *ds;
	DS_CHECK(DirectSoundCreate8(NULL,&ds,NULL));
	DS_CHECK(ds->SetCooperativeLevel(hwnd,DSSCL_PRIORITY));
	// DS_CHECK(ds->SetCooperativeLevel(hwnd,DSSCL_NORMAL));

	DSBUFFERDESC pri_dsbufdesc={};
	pri_dsbufdesc.dwSize=sizeof(DSBUFFERDESC);
	pri_dsbufdesc.dwFlags=DSBCAPS_PRIMARYBUFFER;
	pri_dsbufdesc.dwBufferBytes=0;
	pri_dsbufdesc.dwReserved=0;
	pri_dsbufdesc.lpwfxFormat=NULL;

	struct IDirectSoundBuffer *pri_dsbuf;
	DS_CHECK(ds->CreateSoundBuffer(&pri_dsbufdesc,&pri_dsbuf,NULL));

	WAVEFORMATEX wavfmt={};
	wavfmt.wFormatTag=WAVE_FORMAT_PCM;    
	wavfmt.nChannels=nchans;     
	wavfmt.nSamplesPerSec=smp_per_sec;
	wavfmt.wBitsPerSample=bits_per_mono_samp;
	wavfmt.nBlockAlign=(wavfmt.wBitsPerSample*wavfmt.nChannels)/8u;   
	wavfmt.nAvgBytesPerSec=wavfmt.nBlockAlign*wavfmt.nSamplesPerSec;
	wavfmt.cbSize=0;        

	DS_CHECK(pri_dsbuf->SetFormat(&wavfmt));
	
	DSBUFFERDESC sec_dsbufdesc={};
	sec_dsbufdesc.dwSize=sizeof(DSBUFFERDESC);
	sec_dsbufdesc.dwFlags=0;
	sec_dsbufdesc.dwBufferBytes=(uint32_t)data_size;
	sec_dsbufdesc.dwReserved=0;
	sec_dsbufdesc.lpwfxFormat=&wavfmt;

	struct IDirectSoundBuffer *sec_dsbuf;
	DS_CHECK(ds->CreateSoundBuffer(&sec_dsbufdesc,&sec_dsbuf,NULL));

	unsigned long play_cursor;
	unsigned long write_cursor;
	DS_CHECK(sec_dsbuf->GetCurrentPosition(&play_cursor,&write_cursor));

	void *reg_ptrs[2]={NULL,NULL};
	unsigned long reg_sizes[2];
	DS_CHECK(sec_dsbuf->Lock(
		0,
		sec_dsbufdesc.dwBufferBytes,
		&reg_ptrs[0],
		&reg_sizes[0],
		&reg_ptrs[1],
		&reg_sizes[1],
		DSBLOCK_ENTIREBUFFER
	));

	assert(reg_ptrs[0]);

	
	double t0=itime();

	typedef struct{
		int16_t left;
		int16_t right;
	}sample;

	sample *buf=(sample*)reg_ptrs[0];
	memset(buf,0,data_size);

	float *fbuf=(float*)reg_ptrs[0];//malloc(reg_sizes[0]);
	// memset(fbuf,0,reg_sizes[0]);

	for(uint64_t i=0;i<messages.size();i++){
		key_volumes[messages[i].key]=messages[i].volume/127.0f;

		uint32_t smp_max;
		if(i+1<messages.size()) smp_max=messages[i+1].time*smp_per_tic;
		else smp_max=messages[i].time*smp_per_tic;
		assert(smp_max<=data_size/sizeof(sample));
		const uint32_t smp_min=messages[i].time*smp_per_tic;
		if(smp_min==smp_max) continue;
		assert(smp_min<smp_max);

		

		for(int key=0;key<128;key++) if(fabsf(key_volumes[key])>1e-6){
			const float vol=key_volumes[key];
			const float f=freqs[key];
			for(uint32_t smp=smp_min;smp<smp_max;smp++){
				const float t=(float)smp/(float)smp_per_sec;

				[[maybe_unused]]
				static auto squaref=[](float _t)->float{
					return (float)(2*(((int)(_t*2))%2)-1);
				};

				[[maybe_unused]]
				static auto sawf=[](float _t)->float{
					return 2*fmodf(_t,1)-1;
				};

				[[maybe_unused]]
				static auto trigf=[](float _t)->float{
					float x=fmodf(_t,1);
					float y=fmodf(_t,.5);
					float z=fmodf(_t,.25);
					if(y>.25) z=1-z;
					if(x>.5) z*=-1;
					return z;
				};

				[[maybe_unused]]
				static auto testf=[](float _t)->float{
					return 	1/1.0f*sinf(2*pi*1*_t)+
							1/2.0f*sinf(2*pi*2*_t)+
							1/3.0f*sinf(2*pi*3*_t)+
							1/4.0f*sinf(2*pi*4*_t)+
							1/5.0f*sinf(2*pi*5*_t);
				};

				// const float wave=sinf(2*pi*f*t);
				// const float wave=squaref(f*t);
				// const float wave=sawf(f*t);
				// const float wave=trigf(f*t);
				const float wave=testf(f*t);
				fbuf[smp]+=vol*wave;
			}
			key_volumes[key]*=.9f;
		}
	}
	for(uint32_t i=0;i<reg_sizes[0]/4;i++){
		float x=0x7fff*2*atanf(fbuf[i])/pi;
		buf[i].left =(int16_t)x;
		buf[i].right=(int16_t)x;
	}
	fbuf=NULL;

	double t1=itime();
	PRINT(t1-t0,"%f");
	
	DS_CHECK(sec_dsbuf->Unlock(reg_ptrs[0],reg_sizes[0],reg_ptrs[1],reg_sizes[1]));

	
	// return 0;
	
	DS_CHECK(sec_dsbuf->Play(0,0,DSBPLAY_LOOPING));

   
	MSG msg;
	while(GetMessage(&msg,NULL,0,0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	puts("done");
}
