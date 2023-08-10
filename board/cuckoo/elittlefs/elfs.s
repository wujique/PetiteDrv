	AREA elfs_section, DATA, READONLY
	EXPORT elfsimage
	
elfsimage
	INCBIN elfs.bin
elfsimage_End

elfsimage_length
	DCD elfsimage_End - elfsimage
	
	EXPORT elfsimage_length
	
	END