/*
 *  Copyright (C) 2010, 2011 Stephen F. Booth <me@sbooth.org>
 *  All Rights Reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *    - Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    - Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    - Neither the name of Stephen F. Booth nor the names of its 
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "AddXiphCommentToDictionary.h"
#include "AudioMetadata.h"

bool
AddXiphCommentToDictionary(CFMutableDictionaryRef dictionary, const TagLib::Ogg::XiphComment *tag)
{
	assert(NULL != dictionary);
	assert(NULL != tag);
	
	CFMutableDictionaryRef additionalMetadata = CFDictionaryCreateMutable(kCFAllocatorDefault, 
																		  32,
																		  &kCFTypeDictionaryKeyCallBacks,
																		  &kCFTypeDictionaryValueCallBacks);
	
	// A map <String, StringList>
	TagLib::Ogg::FieldListMap fieldList = tag->fieldListMap();
	
	for(TagLib::Ogg::FieldListMap::ConstIterator it = fieldList.begin(); it != fieldList.end(); ++it) {
		CFStringRef key = CFStringCreateWithCString(kCFAllocatorDefault,
													it->first.toCString(true),
													kCFStringEncodingUTF8);
		
		// Vorbis allows multiple comments with the same key, but this isn't supported by AudioMetadata
		CFStringRef value = CFStringCreateWithCString(kCFAllocatorDefault,
													  it->second.front().toCString(true),
													  kCFStringEncodingUTF8);
		
		if(kCFCompareEqualTo == CFStringCompare(key, CFSTR("ALBUM"), kCFCompareCaseInsensitive))
			CFDictionarySetValue(dictionary, kMetadataAlbumTitleKey, value);
		else if(kCFCompareEqualTo == CFStringCompare(key, CFSTR("ARTIST"), kCFCompareCaseInsensitive))
			CFDictionarySetValue(dictionary, kMetadataArtistKey, value);
		else if(kCFCompareEqualTo == CFStringCompare(key, CFSTR("ALBUMARTIST"), kCFCompareCaseInsensitive))
			CFDictionarySetValue(dictionary, kMetadataAlbumArtistKey, value);
		else if(kCFCompareEqualTo == CFStringCompare(key, CFSTR("COMPOSER"), kCFCompareCaseInsensitive))
			CFDictionarySetValue(dictionary, kMetadataComposerKey, value);
		else if(kCFCompareEqualTo == CFStringCompare(key, CFSTR("GENRE"), kCFCompareCaseInsensitive))
			CFDictionarySetValue(dictionary, kMetadataGenreKey, value);
		else if(kCFCompareEqualTo == CFStringCompare(key, CFSTR("DATE"), kCFCompareCaseInsensitive))
			CFDictionarySetValue(dictionary, kMetadataReleaseDateKey, value);
		else if(kCFCompareEqualTo == CFStringCompare(key, CFSTR("DESCRIPTION"), kCFCompareCaseInsensitive))
			CFDictionarySetValue(dictionary, kMetadataCommentKey, value);
		else if(kCFCompareEqualTo == CFStringCompare(key, CFSTR("TITLE"), kCFCompareCaseInsensitive))
			CFDictionarySetValue(dictionary, kMetadataTitleKey, value);
		else if(kCFCompareEqualTo == CFStringCompare(key, CFSTR("TRACKNUMBER"), kCFCompareCaseInsensitive)) {
			int num = CFStringGetIntValue(value);
			CFNumberRef number = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &num);
			CFDictionarySetValue(dictionary, kMetadataTrackNumberKey, number);
			CFRelease(number), number = NULL;
		}
		else if(kCFCompareEqualTo == CFStringCompare(key, CFSTR("TRACKTOTAL"), kCFCompareCaseInsensitive)) {
			int num = CFStringGetIntValue(value);
			CFNumberRef number = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &num);
			CFDictionarySetValue(dictionary, kMetadataTrackTotalKey, number);
			CFRelease(number), number = NULL;
		}
		else if(kCFCompareEqualTo == CFStringCompare(key, CFSTR("COMPILATION"), kCFCompareCaseInsensitive))
			CFDictionarySetValue(dictionary, kMetadataCompilationKey, CFStringGetIntValue(value) ? kCFBooleanTrue : kCFBooleanFalse);
		else if(kCFCompareEqualTo == CFStringCompare(key, CFSTR("DISCNUMBER"), kCFCompareCaseInsensitive)) {
			int num = CFStringGetIntValue(value);
			CFNumberRef number = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &num);
			CFDictionarySetValue(dictionary, kMetadataDiscNumberKey, number);
			CFRelease(number), number = NULL;
		}
		else if(kCFCompareEqualTo == CFStringCompare(key, CFSTR("DISCTOTAL"), kCFCompareCaseInsensitive)) {
			int num = CFStringGetIntValue(value);
			CFNumberRef number = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &num);
			CFDictionarySetValue(dictionary, kMetadataDiscTotalKey, number);
			CFRelease(number), number = NULL;
		}
		else if(kCFCompareEqualTo == CFStringCompare(key, CFSTR("ISRC"), kCFCompareCaseInsensitive))
			CFDictionarySetValue(dictionary, kMetadataISRCKey, value);
		else if(kCFCompareEqualTo == CFStringCompare(key, CFSTR("MCN"), kCFCompareCaseInsensitive))
			CFDictionarySetValue(dictionary, kMetadataMCNKey, value);
		else if(kCFCompareEqualTo == CFStringCompare(key, CFSTR("REPLAYGAIN_REFERENCE_LOUDNESS"), kCFCompareCaseInsensitive)) {
			double num = CFStringGetDoubleValue(value);
			CFNumberRef number = CFNumberCreate(kCFAllocatorDefault, kCFNumberDoubleType, &num);
			CFDictionarySetValue(dictionary, kReplayGainReferenceLoudnessKey, number);
			CFRelease(number), number = NULL;
		}
		else if(kCFCompareEqualTo == CFStringCompare(key, CFSTR("REPLAYGAIN_TRACK_GAIN"), kCFCompareCaseInsensitive)) {
			double num = CFStringGetDoubleValue(value);
			CFNumberRef number = CFNumberCreate(kCFAllocatorDefault, kCFNumberDoubleType, &num);
			CFDictionarySetValue(dictionary, kReplayGainTrackGainKey, number);
			CFRelease(number), number = NULL;
		}
		else if(kCFCompareEqualTo == CFStringCompare(key, CFSTR("REPLAYGAIN_TRACK_PEAK"), kCFCompareCaseInsensitive)) {
			double num = CFStringGetDoubleValue(value);
			CFNumberRef number = CFNumberCreate(kCFAllocatorDefault, kCFNumberDoubleType, &num);
			CFDictionarySetValue(dictionary, kReplayGainTrackPeakKey, number);
			CFRelease(number), number = NULL;
		}
		else if(kCFCompareEqualTo == CFStringCompare(key, CFSTR("REPLAYGAIN_ALBUM_GAIN"), kCFCompareCaseInsensitive)) {
			double num = CFStringGetDoubleValue(value);
			CFNumberRef number = CFNumberCreate(kCFAllocatorDefault, kCFNumberDoubleType, &num);
			CFDictionarySetValue(dictionary, kReplayGainAlbumGainKey, number);
			CFRelease(number), number = NULL;
		}
		else if(kCFCompareEqualTo == CFStringCompare(key, CFSTR("REPLAYGAIN_ALBUM_PEAK"), kCFCompareCaseInsensitive)) {
			double num = CFStringGetDoubleValue(value);
			CFNumberRef number = CFNumberCreate(kCFAllocatorDefault, kCFNumberDoubleType, &num);
			CFDictionarySetValue(dictionary, kReplayGainAlbumPeakKey, number);
			CFRelease(number), number = NULL;
		}
		// Put all unknown tags into the additional metadata
		else
			CFDictionarySetValue(additionalMetadata, key, value);
		
		CFRelease(key), key = NULL;
		CFRelease(value), value = NULL;
	}
	
	if(CFDictionaryGetCount(additionalMetadata))
		CFDictionarySetValue(dictionary, kMetadataAdditionalMetadataKey, additionalMetadata);
	
	CFRelease(additionalMetadata), additionalMetadata = NULL;
	
	return true;
}
