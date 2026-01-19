#ifndef SOUND_H
#define SOUND_H

#define BGM_PASSOU 0

extern const void* bgm_music_data;

#define SFX_EXPLOSION 0
#define SFX_BLASTER 1

extern const void* sounds;

#define SFX_CHANNEL 0

__attribute__((leaf)) void __music_init(const void *music_data);
__attribute__((leaf)) void FamiToneMusicPlay(char song);
__attribute__((leaf)) void FamiToneSfxInit(const void *sounds_data);
__attribute__((leaf)) void FamiToneSfxPlay(char sound, char channel);

#define music_init __music_init
#define music_play FamiToneMusicPlay
#define sfx_init FamiToneSfxInit
#define sfx_play FamiToneSfxPlay

#endif
