ebml_element ebml_spec[SPEC_LEN] = {
	ebml_element("EBML", {{0x1a, 0x45, 0xdf, 0xa3}}, MASTER),
	ebml_element("EBMLVersion", {{0x42, 0x86}}, UINT),
	ebml_element("EBMLReadVersion", {{0x42, 0xf7}}, UINT),
	ebml_element("EBMLMaxIDLength", {{0x42, 0xf2}}, UINT),
	ebml_element("EBMLMaxSizeLength", {{0x42, 0xf3}}, UINT),
	ebml_element("DocType", {{0x42, 0x82}}, STRING),
	ebml_element("DocTypeVersion", {{0x42, 0x87}}, UINT),
	ebml_element("DocTypeReadVersion", {{0x42, 0x85}}, UINT),
	ebml_element("Void", {{0xec}}, BINARY),
	ebml_element("CRC-32", {{0xbf}}, BINARY),
	ebml_element("SignatureSlot", {{0x1b, 0x53, 0x86, 0x67}}, MASTER),
	ebml_element("SignatureAlgo", {{0x7e, 0x8a}}, UINT),
	ebml_element("SignatureHash", {{0x7e, 0x9a}}, UINT),
	ebml_element("SignaturePublicKey", {{0x7e, 0xa5}}, BINARY),
	ebml_element("Signature", {{0x7e, 0xb5}}, BINARY),
	ebml_element("SignatureElements", {{0x7e, 0x5b}}, MASTER),
	ebml_element("SignatureElementList", {{0x7e, 0x7b}}, MASTER),
	ebml_element("SignedElement", {{0x65, 0x32}}, BINARY),
	ebml_element("Segment", {{0x18, 0x53, 0x80, 0x67}}, MASTER),
	ebml_element("SeekHead", {{0x11, 0x4d, 0x9b, 0x74}}, MASTER),
	ebml_element("Seek", {{0x4d, 0xbb}}, MASTER),
	ebml_element("SeekID", {{0x53, 0xab}}, BINARY),
	ebml_element("SeekPosition", {{0x53, 0xac}}, UINT),
	ebml_element("Info", {{0x15, 0x49, 0xa9, 0x66}}, MASTER),
	ebml_element("SegmentUID", {{0x73, 0xa4}}, BINARY),
	ebml_element("SegmentFilename", {{0x73, 0x84}}, UTF8),
	ebml_element("PrevUID", {{0x3c, 0xb9, 0x23}}, BINARY),
	ebml_element("PrevFilename", {{0x3c, 0x83, 0xab}}, UTF8),
	ebml_element("NextUID", {{0x3e, 0xb9, 0x23}}, BINARY),
	ebml_element("NextFilename", {{0x3e, 0x83, 0xbb}}, UTF8),
	ebml_element("SegmentFamily", {{0x44, 0x44}}, BINARY),
	ebml_element("ChapterTranslate", {{0x69, 0x24}}, MASTER),
	ebml_element("ChapterTranslateEditionUID", {{0x69, 0xfc}}, UINT),
	ebml_element("ChapterTranslateCodec", {{0x69, 0xbf}}, UINT),
	ebml_element("ChapterTranslateID", {{0x69, 0xa5}}, BINARY),
	ebml_element("TimecodeScale", {{0x2a, 0xd7, 0xb1}}, UINT),
	ebml_element("Duration", {{0x44, 0x89}}, FLOAT),
	ebml_element("DateUTC", {{0x44, 0x61}}, DATE),
	ebml_element("Title", {{0x7b, 0xa9}}, UTF8),
	ebml_element("MuxingApp", {{0x4d, 0x80}}, UTF8),
	ebml_element("WritingApp", {{0x57, 0x41}}, UTF8),
	ebml_element("Cluster", {{0x1f, 0x43, 0xb6, 0x75}}, MASTER),
	ebml_element("Timecode", {{0xe7}}, UINT),
	ebml_element("SilentTracks", {{0x58, 0x54}}, MASTER),
	ebml_element("SilentTrackNumber", {{0x58, 0xd7}}, UINT),
	ebml_element("Position", {{0xa7}}, UINT),
	ebml_element("PrevSize", {{0xab}}, UINT),
	ebml_element("SimpleBlock", {{0xa3}}, BINARY),
	ebml_element("BlockGroup", {{0xa0}}, MASTER),
	ebml_element("Block", {{0xa1}}, BINARY),
	ebml_element("BlockVirtual", {{0xa2}}, BINARY),
	ebml_element("BlockAdditions", {{0x75, 0xa1}}, MASTER),
	ebml_element("BlockMore", {{0xa6}}, MASTER),
	ebml_element("BlockAddID", {{0xee}}, UINT),
	ebml_element("BlockAdditional", {{0xa5}}, BINARY),
	ebml_element("BlockDuration", {{0x9b}}, UINT),
	ebml_element("ReferencePriority", {{0xfa}}, UINT),
	ebml_element("ReferenceBlock", {{0xfb}}, INT),
	ebml_element("ReferenceVirtual", {{0xfd}}, INT),
	ebml_element("CodecState", {{0xa4}}, BINARY),
	ebml_element("DiscardPadding", {{0x75, 0xa2}}, INT),
	ebml_element("Slices", {{0x8e}}, MASTER),
	ebml_element("TimeSlice", {{0xe8}}, MASTER),
	ebml_element("LaceNumber", {{0xcc}}, UINT),
	ebml_element("FrameNumber", {{0xcd}}, UINT),
	ebml_element("BlockAdditionID", {{0xcb}}, UINT),
	ebml_element("Delay", {{0xce}}, UINT),
	ebml_element("SliceDuration", {{0xcf}}, UINT),
	ebml_element("ReferenceFrame", {{0xc8}}, MASTER),
	ebml_element("ReferenceOffset", {{0xc9}}, UINT),
	ebml_element("ReferenceTimeCode", {{0xca}}, UINT),
	ebml_element("EncryptedBlock", {{0xaf}}, BINARY),
	ebml_element("Tracks", {{0x16, 0x54, 0xae, 0x6b}}, MASTER),
	ebml_element("TrackEntry", {{0xae}}, MASTER),
	ebml_element("TrackNumber", {{0xd7}}, UINT),
	ebml_element("TrackUID", {{0x73, 0xc5}}, UINT),
	ebml_element("TrackType", {{0x83}}, UINT),
	ebml_element("FlagEnabled", {{0xb9}}, UINT),
	ebml_element("FlagDefault", {{0x88}}, UINT),
	ebml_element("FlagForced", {{0x55, 0xaa}}, UINT),
	ebml_element("FlagLacing", {{0x9c}}, UINT),
	ebml_element("MinCache", {{0x6d, 0xe7}}, UINT),
	ebml_element("MaxCache", {{0x6d, 0xf8}}, UINT),
	ebml_element("DefaultDuration", {{0x23, 0xe3, 0x83}}, UINT),
	ebml_element("DefaultDecodedFieldDuration", {{0x23, 0x4e, 0x7a}}, UINT),
	ebml_element("TrackTimecodeScale", {{0x23, 0x31, 0x4f}}, FLOAT),
	ebml_element("TrackOffset", {{0x53, 0x7f}}, INT),
	ebml_element("MaxBlockAdditionID", {{0x55, 0xee}}, UINT),
	ebml_element("Name", {{0x53, 0x6e}}, UTF8),
	ebml_element("Language", {{0x22, 0xb5, 0x9c}}, STRING),
	ebml_element("CodecID", {{0x86}}, STRING),
	ebml_element("CodecPrivate", {{0x63, 0xa2}}, BINARY),
	ebml_element("CodecName", {{0x25, 0x86, 0x88}}, UTF8),
	ebml_element("AttachmentLink", {{0x74, 0x46}}, UINT),
	ebml_element("CodecSettings", {{0x3a, 0x96, 0x97}}, UTF8),
	ebml_element("CodecInfoURL", {{0x3b, 0x40, 0x40}}, STRING),
	ebml_element("CodecDownloadURL", {{0x26, 0xb2, 0x40}}, STRING),
	ebml_element("CodecDecodeAll", {{0xaa}}, UINT),
	ebml_element("TrackOverlay", {{0x6f, 0xab}}, UINT),
	ebml_element("CodecDelay", {{0x56, 0xaa}}, UINT),
	ebml_element("SeekPreRoll", {{0x56, 0xbb}}, UINT),
	ebml_element("TrackTranslate", {{0x66, 0x24}}, MASTER),
	ebml_element("TrackTranslateEditionUID", {{0x66, 0xfc}}, UINT),
	ebml_element("TrackTranslateCodec", {{0x66, 0xbf}}, UINT),
	ebml_element("TrackTranslateTrackID", {{0x66, 0xa5}}, BINARY),
	ebml_element("Video", {{0xe0}}, MASTER),
	ebml_element("FlagInterlaced", {{0x9a}}, UINT),
	ebml_element("FieldOrder", {{0x9d}}, UINT),
	ebml_element("StereoMode", {{0x53, 0xb8}}, UINT),
	ebml_element("AlphaMode", {{0x53, 0xc0}}, UINT),
	ebml_element("OldStereoMode", {{0x53, 0xb9}}, UINT),
	ebml_element("PixelWidth", {{0xb0}}, UINT),
	ebml_element("PixelHeight", {{0xba}}, UINT),
	ebml_element("PixelCropBottom", {{0x54, 0xaa}}, UINT),
	ebml_element("PixelCropTop", {{0x54, 0xbb}}, UINT),
	ebml_element("PixelCropLeft", {{0x54, 0xcc}}, UINT),
	ebml_element("PixelCropRight", {{0x54, 0xdd}}, UINT),
	ebml_element("DisplayWidth", {{0x54, 0xb0}}, UINT),
	ebml_element("DisplayHeight", {{0x54, 0xba}}, UINT),
	ebml_element("DisplayUnit", {{0x54, 0xb2}}, UINT),
	ebml_element("AspectRatioType", {{0x54, 0xb3}}, UINT),
	ebml_element("ColourSpace", {{0x2e, 0xb5, 0x24}}, BINARY),
	ebml_element("GammaValue", {{0x2f, 0xb5, 0x23}}, FLOAT),
	ebml_element("FrameRate", {{0x23, 0x83, 0xe3}}, FLOAT),
	ebml_element("Colour", {{0x55, 0xb0}}, MASTER),
	ebml_element("MatrixCoefficients", {{0x55, 0xb1}}, UINT),
	ebml_element("BitsPerChannel", {{0x55, 0xb2}}, UINT),
	ebml_element("ChromaSubsamplingHorz", {{0x55, 0xb3}}, UINT),
	ebml_element("ChromaSubsamplingVert", {{0x55, 0xb4}}, UINT),
	ebml_element("CbSubsamplingHorz", {{0x55, 0xb5}}, UINT),
	ebml_element("CbSubsamplingVert", {{0x55, 0xb6}}, UINT),
	ebml_element("ChromaSitingHorz", {{0x55, 0xb7}}, UINT),
	ebml_element("ChromaSitingVert", {{0x55, 0xb8}}, UINT),
	ebml_element("Range", {{0x55, 0xb9}}, UINT),
	ebml_element("TransferCharacteristics", {{0x55, 0xba}}, UINT),
	ebml_element("Primaries", {{0x55, 0xbb}}, UINT),
	ebml_element("MaxCLL", {{0x55, 0xbc}}, UINT),
	ebml_element("MaxFALL", {{0x55, 0xbd}}, UINT),
	ebml_element("MasteringMetadata", {{0x55, 0xd0}}, MASTER),
	ebml_element("PrimaryRChromaticityX", {{0x55, 0xd1}}, FLOAT),
	ebml_element("PrimaryRChromaticityY", {{0x55, 0xd2}}, FLOAT),
	ebml_element("PrimaryGChromaticityX", {{0x55, 0xd3}}, FLOAT),
	ebml_element("PrimaryGChromaticityY", {{0x55, 0xd4}}, FLOAT),
	ebml_element("PrimaryBChromaticityX", {{0x55, 0xd5}}, FLOAT),
	ebml_element("PrimaryBChromaticityY", {{0x55, 0xd6}}, FLOAT),
	ebml_element("WhitePointChromaticityX", {{0x55, 0xd7}}, FLOAT),
	ebml_element("WhitePointChromaticityY", {{0x55, 0xd8}}, FLOAT),
	ebml_element("LuminanceMax", {{0x55, 0xd9}}, FLOAT),
	ebml_element("LuminanceMin", {{0x55, 0xda}}, FLOAT),
	ebml_element("Audio", {{0xe1}}, MASTER),
	ebml_element("SamplingFrequency", {{0xb5}}, FLOAT),
	ebml_element("OutputSamplingFrequency", {{0x78, 0xb5}}, FLOAT),
	ebml_element("Channels", {{0x9f}}, UINT),
	ebml_element("ChannelPositions", {{0x7d, 0x7b}}, BINARY),
	ebml_element("BitDepth", {{0x62, 0x64}}, UINT),
	ebml_element("TrackOperation", {{0xe2}}, MASTER),
	ebml_element("TrackCombinePlanes", {{0xe3}}, MASTER),
	ebml_element("TrackPlane", {{0xe4}}, MASTER),
	ebml_element("TrackPlaneUID", {{0xe5}}, UINT),
	ebml_element("TrackPlaneType", {{0xe6}}, UINT),
	ebml_element("TrackJoinBlocks", {{0xe9}}, MASTER),
	ebml_element("TrackJoinUID", {{0xed}}, UINT),
	ebml_element("TrickTrackUID", {{0xc0}}, UINT),
	ebml_element("TrickTrackSegmentUID", {{0xc1}}, BINARY),
	ebml_element("TrickTrackFlag", {{0xc6}}, UINT),
	ebml_element("TrickMasterTrackUID", {{0xc7}}, UINT),
	ebml_element("TrickMasterTrackSegmentUID", {{0xc4}}, BINARY),
	ebml_element("ContentEncodings", {{0x6d, 0x80}}, MASTER),
	ebml_element("ContentEncoding", {{0x62, 0x40}}, MASTER),
	ebml_element("ContentEncodingOrder", {{0x50, 0x31}}, UINT),
	ebml_element("ContentEncodingScope", {{0x50, 0x32}}, UINT),
	ebml_element("ContentEncodingType", {{0x50, 0x33}}, UINT),
	ebml_element("ContentCompression", {{0x50, 0x34}}, MASTER),
	ebml_element("ContentCompAlgo", {{0x42, 0x54}}, UINT),
	ebml_element("ContentCompSettings", {{0x42, 0x55}}, BINARY),
	ebml_element("ContentEncryption", {{0x50, 0x35}}, MASTER),
	ebml_element("ContentEncAlgo", {{0x47, 0xe1}}, UINT),
	ebml_element("ContentEncKeyID", {{0x47, 0xe2}}, BINARY),
	ebml_element("ContentSignature", {{0x47, 0xe3}}, BINARY),
	ebml_element("ContentSigKeyID", {{0x47, 0xe4}}, BINARY),
	ebml_element("ContentSigAlgo", {{0x47, 0xe5}}, UINT),
	ebml_element("ContentSigHashAlgo", {{0x47, 0xe6}}, UINT),
	ebml_element("Cues", {{0x1c, 0x53, 0xbb, 0x6b}}, MASTER),
	ebml_element("CuePoint", {{0xbb}}, MASTER),
	ebml_element("CueTime", {{0xb3}}, UINT),
	ebml_element("CueTrackPositions", {{0xb7}}, MASTER),
	ebml_element("CueTrack", {{0xf7}}, UINT),
	ebml_element("CueClusterPosition", {{0xf1}}, UINT),
	ebml_element("CueRelativePosition", {{0xf0}}, UINT),
	ebml_element("CueDuration", {{0xb2}}, UINT),
	ebml_element("CueBlockNumber", {{0x53, 0x78}}, UINT),
	ebml_element("CueCodecState", {{0xea}}, UINT),
	ebml_element("CueReference", {{0xdb}}, MASTER),
	ebml_element("CueRefTime", {{0x96}}, UINT),
	ebml_element("CueRefCluster", {{0x97}}, UINT),
	ebml_element("CueRefNumber", {{0x53, 0x5f}}, UINT),
	ebml_element("CueRefCodecState", {{0xeb}}, UINT),
	ebml_element("Attachments", {{0x19, 0x41, 0xa4, 0x69}}, MASTER),
	ebml_element("AttachedFile", {{0x61, 0xa7}}, MASTER),
	ebml_element("FileDescription", {{0x46, 0x7e}}, UTF8),
	ebml_element("FileName", {{0x46, 0x6e}}, UTF8),
	ebml_element("FileMimeType", {{0x46, 0x60}}, STRING),
	ebml_element("FileData", {{0x46, 0x5c}}, BINARY),
	ebml_element("FileUID", {{0x46, 0xae}}, UINT),
	ebml_element("FileReferral", {{0x46, 0x75}}, BINARY),
	ebml_element("FileUsedStartTime", {{0x46, 0x61}}, UINT),
	ebml_element("FileUsedEndTime", {{0x46, 0x62}}, UINT),
	ebml_element("Chapters", {{0x10, 0x43, 0xa7, 0x70}}, MASTER),
	ebml_element("EditionEntry", {{0x45, 0xb9}}, MASTER),
	ebml_element("EditionUID", {{0x45, 0xbc}}, UINT),
	ebml_element("EditionFlagHidden", {{0x45, 0xbd}}, UINT),
	ebml_element("EditionFlagDefault", {{0x45, 0xdb}}, UINT),
	ebml_element("EditionFlagOrdered", {{0x45, 0xdd}}, UINT),
	ebml_element("ChapterAtom", {{0xb6}}, MASTER),
	ebml_element("ChapterUID", {{0x73, 0xc4}}, UINT),
	ebml_element("ChapterStringUID", {{0x56, 0x54}}, UTF8),
	ebml_element("ChapterTimeStart", {{0x91}}, UINT),
	ebml_element("ChapterTimeEnd", {{0x92}}, UINT),
	ebml_element("ChapterFlagHidden", {{0x98}}, UINT),
	ebml_element("ChapterFlagEnabled", {{0x45, 0x98}}, UINT),
	ebml_element("ChapterSegmentUID", {{0x6e, 0x67}}, BINARY),
	ebml_element("ChapterSegmentEditionUID", {{0x6e, 0xbc}}, UINT),
	ebml_element("ChapterPhysicalEquiv", {{0x63, 0xc3}}, UINT),
	ebml_element("ChapterTrack", {{0x8f}}, MASTER),
	ebml_element("ChapterTrackNumber", {{0x89}}, UINT),
	ebml_element("ChapterDisplay", {{0x80}}, MASTER),
	ebml_element("ChapString", {{0x85}}, UTF8),
	ebml_element("ChapLanguage", {{0x43, 0x7c}}, STRING),
	ebml_element("ChapCountry", {{0x43, 0x7e}}, STRING),
	ebml_element("ChapProcess", {{0x69, 0x44}}, MASTER),
	ebml_element("ChapProcessCodecID", {{0x69, 0x55}}, UINT),
	ebml_element("ChapProcessPrivate", {{0x45, 0x0d}}, BINARY),
	ebml_element("ChapProcessCommand", {{0x69, 0x11}}, MASTER),
	ebml_element("ChapProcessTime", {{0x69, 0x22}}, UINT),
	ebml_element("ChapProcessData", {{0x69, 0x33}}, BINARY),
	ebml_element("Tags", {{0x12, 0x54, 0xc3, 0x67}}, MASTER),
	ebml_element("Tag", {{0x73, 0x73}}, MASTER),
	ebml_element("Targets", {{0x63, 0xc0}}, MASTER),
	ebml_element("TargetTypeValue", {{0x68, 0xca}}, UINT),
	ebml_element("TargetType", {{0x63, 0xca}}, STRING),
	ebml_element("TagTrackUID", {{0x63, 0xc5}}, UINT),
	ebml_element("TagEditionUID", {{0x63, 0xc9}}, UINT),
	ebml_element("TagChapterUID", {{0x63, 0xc4}}, UINT),
	ebml_element("TagAttachmentUID", {{0x63, 0xc6}}, UINT),
	ebml_element("SimpleTag", {{0x67, 0xc8}}, MASTER),
	ebml_element("TagName", {{0x45, 0xa3}}, UTF8),
	ebml_element("TagLanguage", {{0x44, 0x7a}}, STRING),
	ebml_element("TagDefault", {{0x44, 0x84}}, UINT),
	ebml_element("TagString", {{0x44, 0x87}}, UTF8),
	ebml_element("TagBinary", {{0x44, 0x85}}, BINARY)
};
