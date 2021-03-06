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

#include <log4cxx/logger.h>

#include "AudioMetadata.h"
#include "SetXiphCommentFromMetadata.h"
#include "TagLibStringFromCFString.h"

// ========================================
// Xiph comment utilities
// ========================================
static bool
SetXiphComment(TagLib::Ogg::XiphComment *tag, const char *key, CFStringRef value)
{
	assert(NULL != tag);
	assert(NULL != key);
	
	// Remove the existing comment with this name
	tag->removeField(key);
	
	// Nothing left to do if value is NULL
	if(NULL == value)
		return true;
	
	tag->addField(key, TagLib::StringFromCFString(value));
	
	return true;
}

static bool
SetXiphCommentNumber(TagLib::Ogg::XiphComment *tag, const char *key, CFNumberRef value)
{
	assert(NULL != tag);
	assert(NULL != key);
	
	CFStringRef numberString = NULL;
	
	if(NULL != value)
		numberString = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("%@"), value);
	
	bool result = SetXiphComment(tag, key, numberString);
	
	if(numberString)
		CFRelease(numberString), numberString = NULL;
	
	return result;
}

static bool
SetXiphCommentBoolean(TagLib::Ogg::XiphComment *tag, const char *key, CFBooleanRef value)
{
	assert(NULL != tag);
	assert(NULL != key);

	if(NULL == value)
		return SetXiphComment(tag, key, NULL);
	else if(CFBooleanGetValue(value))
		return SetXiphComment(tag, key, CFSTR("1"));
	else
		return SetXiphComment(tag, key, CFSTR("0"));
}

static bool
SetXiphCommentDouble(TagLib::Ogg::XiphComment *tag, const char *key, CFNumberRef value, CFStringRef format = NULL)
{
	assert(NULL != tag);
	assert(NULL != key);
	
	CFStringRef numberString = NULL;
	
	if(NULL != value) {
		double f;
		if(!CFNumberGetValue(value, kCFNumberDoubleType, &f)) {
			log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger("org.sbooth.AudioEngine");
			LOG4CXX_ERROR(logger, "CFNumberGetValue failed");
			return false;
		}
		
		numberString = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, NULL == format ? CFSTR("%f") : format, f);
	}
	
	bool result = SetXiphComment(tag, key, numberString);
	
	if(numberString)
		CFRelease(numberString), numberString = NULL;
	
	return result;
}

bool
SetXiphCommentFromMetadata(const AudioMetadata& metadata, TagLib::Ogg::XiphComment *tag)
{
	assert(NULL != tag);

	// Standard tags
	SetXiphComment(tag, "ALBUM", metadata.GetAlbumTitle());
	SetXiphComment(tag, "ARTIST", metadata.GetArtist());
	SetXiphComment(tag, "ALBUMARTIST", metadata.GetAlbumArtist());
	SetXiphComment(tag, "COMPOSER", metadata.GetComposer());
	SetXiphComment(tag, "GENRE", metadata.GetGenre());
	SetXiphComment(tag, "DATE", metadata.GetReleaseDate());
	SetXiphComment(tag, "DESCRIPTION", metadata.GetComment());
	SetXiphComment(tag, "TITLE", metadata.GetTitle());
	SetXiphCommentNumber(tag, "TRACKNUMBER", metadata.GetTrackNumber());
	SetXiphCommentNumber(tag, "TRACKTOTAL", metadata.GetTrackTotal());
	SetXiphCommentBoolean(tag, "COMPILATION", metadata.GetCompilation());
	SetXiphCommentNumber(tag, "DISCNUMBER", metadata.GetDiscNumber());
	SetXiphCommentNumber(tag, "DISCTOTAL", metadata.GetDiscTotal());
	SetXiphComment(tag, "ISRC", metadata.GetISRC());
	SetXiphComment(tag, "MCN", metadata.GetMCN());
	
	// Additional metadata
	CFDictionaryRef additionalMetadata = metadata.GetAdditionalMetadata();
	if(NULL != additionalMetadata) {
		CFIndex count = CFDictionaryGetCount(additionalMetadata);
		
		const void * keys [count];
		const void * values [count];
		
		CFDictionaryGetKeysAndValues(additionalMetadata, reinterpret_cast<const void **>(keys), reinterpret_cast<const void **>(values));
		
		for(CFIndex i = 0; i < count; ++i) {
			CFIndex keySize = CFStringGetMaximumSizeForEncoding(CFStringGetLength(reinterpret_cast<CFStringRef>(keys[i])), kCFStringEncodingASCII);
			char key [keySize + 1];
			
			if(!CFStringGetCString(reinterpret_cast<CFStringRef>(keys[i]), key, keySize + 1, kCFStringEncodingASCII)) {
				log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger("org.sbooth.AudioEngine");
				LOG4CXX_ERROR(logger, "CFStringGetCString failed");
				continue;
			}
			
			SetXiphComment(tag, key, reinterpret_cast<CFStringRef>(values[i]));
		}
	}
	
	// ReplayGain info
	SetXiphCommentDouble(tag, "REPLAYGAIN_REFERENCE_LOUDNESS", metadata.GetReplayGainReferenceLoudness(), CFSTR("%2.1f dB"));
	SetXiphCommentDouble(tag, "REPLAYGAIN_TRACK_GAIN", metadata.GetReplayGainReferenceLoudness(), CFSTR("%+2.2f dB"));
	SetXiphCommentDouble(tag, "REPLAYGAIN_TRACK_PEAK", metadata.GetReplayGainTrackGain(), CFSTR("%1.8f"));
	SetXiphCommentDouble(tag, "REPLAYGAIN_ALBUM_GAIN", metadata.GetReplayGainAlbumGain(), CFSTR("%+2.2f dB"));
	SetXiphCommentDouble(tag, "REPLAYGAIN_ALBUM_PEAK", metadata.GetReplayGainAlbumPeak(), CFSTR("%1.8f"));

	return true;
}
