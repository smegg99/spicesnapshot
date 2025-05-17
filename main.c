// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include "capture.h"

static void print_usage(const char * prog) {
  printf(
    "spicesnapshot version 1.0\n"
    "Usage: %s [OPTIONS] <IP> <PORT> <OUTPUT.png>\n"
    "Options:\n"
    "  -p, --password-file <file>  read VNC password from file\n"
    "  -f, --fps <FPS>             frames per second (default 1)\n"
    "  -c, --count <COUNT>         number of snapshots (default 1)\n"
    "  -v, --verbose               print detailed output\n"
    "  -q, --quiet                 suppress all output except errors\n"
    "  -h, --help                  this help message\n",
    prog
  );
}

static char * load_password(const char * path) {
  FILE * f = fopen(path, "r");
  if (!f) return NULL;
  fseek(f, 0, SEEK_END);
  long sz = ftell(f);
  rewind(f);
  char * buf = malloc(sz + 1);
  if (buf) {
    fread(buf, 1, sz, f);
    buf[sz] = '\0';
    char * nl = strchr(buf, '\n');
    if (nl) * nl = '\0';
  }
  fclose(f);
  return buf;
}

int main(int argc, char * argv[]) {
  int verbose = 0, quiet = 0;
  char * passfile = NULL;
  double fps = 1.0;
  int count = 1;

  struct option longopts[] = {
    {
      "password-file",
      required_argument,
      NULL,
      'p'
    },
    {
      "fps",
      required_argument,
      NULL,
      'f'
    },
    {
      "count",
      required_argument,
      NULL,
      'c'
    },
    {
      "verbose",
      no_argument,
      NULL,
      'v'
    },
    {
      "quiet",
      no_argument,
      NULL,
      'q'
    },
    {
      "help",
      no_argument,
      NULL,
      'h'
    },
    {
      NULL,
      0,
      NULL,
      0
    }
  };

  int opt;
  while ((opt = getopt_long(argc, argv, "p:f:c:vqh", longopts, NULL)) != -1) {
    switch (opt) {
    case 'p':
      passfile = optarg;
      break;
    case 'f':
      fps = atof(optarg);
      break;
    case 'c':
      count = atoi(optarg);
      break;
    case 'v':
      verbose = 1;
      break;
    case 'q':
      quiet = 1;
      verbose = 0;
      break;
    case 'h':
    default:
      print_usage(argv[0]);
      return (opt == 'h' ? 0 : 1);
    }
  }

  if (quiet) {
    freopen("/dev/null", "w", stderr);
  }

  if (optind + 3 > argc) {
    print_usage(argv[0]);
    return 1;
  }

  const char * ip = argv[optind];
  const char * port_s = argv[optind + 1];
  const char * base = argv[optind + 2];

  int port = atoi(port_s);
  if (port == 0 && strcmp(port_s, "0") != 0) {
    if (!quiet) fprintf(stderr, "Invalid port: %s\n", port_s);
    return 1;
  }

  char * password = NULL;
  if (passfile) {
    password = load_password(passfile);
    if (!password) {
      if (!quiet) fprintf(stderr, "Failed to load password from %s\n", passfile);
      return 1;
    }
  }

  for (int i = 0; i < count; i++) {
    char output[1024];
    if (count > 1) {
      snprintf(output, sizeof(output), "%.*s-%d.png",
        (int)(strrchr(base, '.') - base), base, i + 1);
      strcat(output, strrchr(base, '.'));
    } else {
      strncpy(output, base, sizeof(output) - 1);
      output[sizeof(output) - 1] = '\0';
    }

    int ret = capture_spice_snapshot(ip, port, output, password);
    if (ret != 0) {
      if (!quiet) fprintf(stderr, "Snapshot %d failed\n", i + 1);
      free(password);
      return 1;
    }
    if (verbose && !quiet)
      printf("Snapshot %d saved to %s\n", i + 1, output);

    if (i + 1 < count)
      usleep((useconds_t)(1e6 / fps));
  }

  free(password);
  return 0;
}