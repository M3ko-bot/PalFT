/* 
 * Copyright (c) 2024, The PerformanC Organization
 * License available at LICENSE file (BSD 2-Clause)
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if __linux__
  #include <sys/random.h>
#endif
#ifdef _WIN32
  #include <windows.h>
#else
  #if _POSIX_C_SOURCE >= 199309L
    #include <time.h>
  #else
    #include <unistd.h>
  #endif
#endif

unsigned int frequenc_safe_seeding(void) {
  unsigned int seed = 0;
  
  #ifdef _WIN32
    HCRYPTPROV h_crypt_prov;
    if (!CryptAcquireContext(&h_crypt_prov, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
      perror("[utils:Windows]: Failed to acquire cryptographic context");

      exit(1);
    }

    if (!CryptGenRandom(h_crypt_prov, sizeof(seed), (BYTE *) &seed)) {
      CryptReleaseContext(h_crypt_prov, 0);

      perror("[utils:Windows]: Failed to generate random seed");

      exit(1);
    }

    CryptReleaseContext(h_crypt_prov, 0);

    return seed;
  #elif defined __linux__
    if (getrandom(&seed, sizeof(seed), 0) == -1) {
      perror("[utils:Linux]: Failed to generate random seed");

      exit(1);
    }
  #elif ALLOW_UNSECURE_RANDOM
    printf("[utils:Unknown]: Warning! Using unsecure random seed.\n");

    seed = time(NULL);
  #else
    printf("[utils:Unknown]: Warning! Unsupported OS. Using /dev/urandom to generate random seed.\n");

    FILE *urandom = fopen("/dev/urandom", "rb");
    if (urandom == NULL) {
      perror("[utils:Unknown]: Failed to open /dev/urandom");

      exit(1);
    }

    if (fread(&seed, sizeof(seed), 1, urandom) != 1) {
      fclose(urandom);

      perror("[utils:Unknown]: Failed to read from /dev/urandom");

      exit(1);
    }

    fclose(urandom);
  #endif

  return seed;
}

char *frequenc_generate_session_id(char *result) {
  #if HARDCODED_SESSION_ID
    frequenc_fast_copy("FrequenC_Develop", result, 17);

    result[16] = '\0';

    return result;
  #endif

  char characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

  srand(frequenc_safe_seeding());

  int i = 0;
  while (i < 16) {
    result[i] = characters[rand() % ((int)sizeof(characters) - 1)];
    i++;
  }

  result[i] = '\0';

  return result;
}