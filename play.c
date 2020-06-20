/*
 * Attached to our trinket m0 GPIO PIN
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/mman.h>
#include <mad.h>
#include <pulse/simple.h>
#include <pulse/error.h>

#include <wiringPi.h>

#define DELAY_BETWEEN_ALARM 15 // how long to wait before double alarm

#define ALERT_PIN 36
#define INDICATOR_PIN 32  

pa_simple *device = NULL;
int ret = 1;
int error;
struct mad_stream mad_stream;
struct mad_frame mad_frame;
struct mad_synth mad_synth;

void output(struct mad_header const *header, struct mad_pcm *pcm);

// see: https://lauri.xn--vsandi-pxa.com/2013/12/implementing-mp3-player.en.html
int playDoorChim() {
  // Set up PulseAudio 16-bit 44.1kHz stereo output
	static const pa_sample_spec ss = { .format = PA_SAMPLE_S16LE, .rate = 44100, .channels = 2 };
	if (!(device = pa_simple_new(NULL, "MP3 player", PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &error))) {
    printf("pa_simple_new() failed\n");
    return 255;
	}

	// Initialize MAD library
	mad_stream_init(&mad_stream);
	mad_synth_init(&mad_synth);
	mad_frame_init(&mad_frame);

	// Filename pointer
	char *filename = "/home/pi/alert-sensor/doorbell.mp3";

	// File pointer
	FILE *fp = fopen(filename, "r");
	int fd = fileno(fp);

	// Fetch file size, etc
	struct stat metadata;
	if (fstat(fd, &metadata) >= 0) {
    printf("File size %d bytes\n", (int)metadata.st_size);
	} else {
    printf("Failed to stat %s\n", filename);
    fclose(fp);
    return 254;
	}

	// Let kernel do all the dirty job of buffering etc, map file contents to memory
	const unsigned char *input_stream = mmap(0, metadata.st_size, PROT_READ, MAP_SHARED, fd, 0);

	// Copy pointer and length to mad_stream struct
	mad_stream_buffer(&mad_stream, input_stream, metadata.st_size);

  printf("start playback\n");
	// Decode frame and synthesize loop
	while (1) {
		// Decode frame from the stream
		if (mad_frame_decode(&mad_frame, &mad_stream)) {
			if (MAD_RECOVERABLE(mad_stream.error)) {
        continue;
			} else if (mad_stream.error == MAD_ERROR_BUFLEN) {
        break;
			} else {
        break;
			}
		}
		// Synthesize PCM data of frame
		mad_synth_frame(&mad_synth, &mad_frame);
		output(&mad_frame.header, &mad_synth.pcm);
	}
  printf("end playback\n");

	// Close
	fclose(fp);

	// Free MAD structs
	mad_synth_finish(&mad_synth);
	mad_frame_finish(&mad_frame);
	mad_stream_finish(&mad_stream);

	// Close PulseAudio output
	if (device) {
		pa_simple_free(device);
  }
  return EXIT_SUCCESS;
}
// Some helper functions, to be cleaned up in the future
int scale(mad_fixed_t sample) {
  /* round */
  sample += (1L << (MAD_F_FRACBITS - 16));
  /* clip */
  if (sample >= MAD_F_ONE)
    sample = MAD_F_ONE - 1;
  else if (sample < -MAD_F_ONE)
    sample = -MAD_F_ONE;
  /* quantize */
  return sample >> (MAD_F_FRACBITS + 1 - 16);
}

void output(struct mad_header const *header, struct mad_pcm *pcm) {
  register int nsamples = pcm->length;
  mad_fixed_t const *left_ch = pcm->samples[0], *right_ch = pcm->samples[1];
  static char stream[1152*4];
  if (pcm->channels == 2) {
    while (nsamples--) {
      signed int sample;
      sample = scale(*left_ch++);
      stream[(pcm->length-nsamples)*4 ] = ((sample >> 0) & 0xff);
      stream[(pcm->length-nsamples)*4 +1] = ((sample >> 8) & 0xff);
      sample = scale(*right_ch++);
      stream[(pcm->length-nsamples)*4+2 ] = ((sample >> 0) & 0xff);
      stream[(pcm->length-nsamples)*4 +3] = ((sample >> 8) & 0xff);
    }
    if (pa_simple_write(device, stream, (size_t)1152*4, &error) < 0) {
      fprintf(stderr, "pa_simple_write() failed: %s\n", pa_strerror(error));
      return;
    }
  } else {
    printf("Mono not supported!");
  }
}

static time_t lastAlarm = 0;

void setup() {

  lastAlarm = time(0) - DELAY_BETWEEN_ALARM;

  wiringPiSetupPhys();

  pinMode(ALERT_PIN, INPUT);
  pinMode(INDICATOR_PIN, OUTPUT);

}


void loop() {
  int secondsSinceLastAlarm = 0;
  time_t alarmTime = 0;

  int alert = digitalRead(ALERT_PIN);

  if (!alert) {
    alarmTime = time(0);
    secondsSinceLastAlarm = (int)difftime(alarmTime, lastAlarm);
    if (secondsSinceLastAlarm > DELAY_BETWEEN_ALARM) {
      printf("Alarm!\n");
      digitalWrite(INDICATOR_PIN, HIGH);
      lastAlarm = alarmTime;
      playDoorChim();
    } else {
      printf("Last alarmed within the same minute, %d seconds remaining.\n", (DELAY_BETWEEN_ALARM - secondsSinceLastAlarm));
      if ((secondsSinceLastAlarm % 2) == 1) {
        digitalWrite(INDICATOR_PIN, LOW);
      } else {
        digitalWrite(INDICATOR_PIN, HIGH);
      }
      delay(500);
    }
  } else {
    digitalWrite(INDICATOR_PIN, LOW);
  }

}

void cleanup(int signum, siginfo_t *info, void *ptr) {
  digitalWrite(INDICATOR_PIN, LOW);

  exit(0);
}

int main(int argc, char**argv) {
  if (argc > 1) {
    playDoorChim();
    return 0;
  }
  static struct sigaction _sigact;
  memset(&_sigact, 0, sizeof(_sigact));
  _sigact.sa_sigaction = cleanup;
  _sigact.sa_flags = SA_SIGINFO;

  sigaction(SIGINT, &_sigact, NULL);

  setup();

  puts("warming up 3 seconds...");
  delay(3000);
  puts("starting up now");

  while (1) {
    loop();
  }

  return 0;
}
