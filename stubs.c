#include <Security/Security.h>

typedef uint32_t SecTrustSettingsDomain;
typedef uint32_t SecExternalFormat;
typedef uint32_t SecKeyImportExportFlags;
typedef uint32_t SecItemImportExportFlags;

typedef struct {} SecItemImportExportKeyParameters;

OSStatus SecItemExport(CFTypeRef secItemOrArray, SecExternalFormat outputFormat, SecItemImportExportFlags flags, const SecItemImportExportKeyParameters *keyParams, CFDataRef  _Nullable *exportedData) {
    printf("Unimplemented function: SecItemExport\n");
    return errSecUnimplemented;
}

OSStatus SecTrustSettingsCopyCertificates(SecTrustSettingsDomain domain, CFArrayRef  _Nullable *certArray) {
    printf("Unimplemented function: SecTrustSettingsCopyCertificates\n");
    return errSecUnimplemented;
}

OSStatus SecTrustSettingsCopyTrustSettings(
	SecCertificateRef certRef, 
	SecTrustSettingsDomain domain,
	CFArrayRef *trustSettings)				/* RETURNED */
{
    printf("Unimplemented function: SecTrustSettingsCopyTrustSettings\n");
    return errSecUnimplemented;
}
