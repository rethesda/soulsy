//! Character encoding shenanigans. Bethesda is very bad at utf-8, I am told.
use cxx::CxxVector;
use encoding::label::encoding_from_whatwg_label;
use encoding::DecoderTrap;

// To test in game: install daegon
// player.additem 4c2b15f4 1
// Sacrÿfev Tëliimi

/// C++ should use this for std::string conversions.
pub fn string_to_utf8(bytes_ffi: &CxxVector<u8>) -> String {
    let bytes: Vec<u8> = bytes_ffi.iter().copied().collect();
    convert_to_utf8(bytes)
}

/// Use this for null-terminated C strings.
pub fn cstr_to_utf8(bytes_ffi: &CxxVector<u8>) -> String {
    let bytes: Vec<u8> = bytes_ffi.iter().copied().collect();
    let bytes = if bytes.ends_with(&[0]) {
        let chopped = bytes.len() - 1;
        let mut tmp = bytes.clone();
        tmp.truncate(chopped);
        tmp
    } else {
        bytes
    };
    convert_to_utf8(bytes)
}

/// Get a valid Rust representation of this Windows codepage string data by hook or by crook.
pub fn convert_to_utf8(bytes: Vec<u8>) -> String {
    if bytes.is_empty() {
        return String::new();
    }

    let (encoding, confidence, _language) = chardet::detect(&bytes);
    let encoding = if confidence < 0.75 {
        "iso-8859-1".to_string() // yeah, well.
    } else {
        encoding
    };
    if let Some(coder) = encoding_from_whatwg_label(chardet::charset2encoding(&encoding)) {
        if let Ok(utf8string) = coder.decode(&bytes, DecoderTrap::Replace) {
            return utf8string.to_string();
        }
    }

    String::from_utf8_lossy(&bytes).to_string()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn utf8_data_is_untouched() {
        let example = "Sacrÿfev Tëliimi";
        let converted = convert_to_utf8(example.as_bytes().to_vec());
        assert_eq!(converted, example);
        let ex2 = "おはよう";
        let convert2 = convert_to_utf8(ex2.as_bytes().to_vec());
        assert_eq!(convert2, ex2);
        let ex3 = "Zażółć gęślą jaźń";
        let convert3 = convert_to_utf8(ex3.as_bytes().to_vec());
        assert_eq!(convert3, ex3);
    }

    #[test]
    fn iso8859_is_decoded() {
        // This is the example above (from the Daegon mod), in its expression
        // as windows codepage bytes. This test is the equivalent of me testing
        // that the textcode mod works, but I am feeling timid.
        let bytes: Vec<u8> = vec![
            0x53, 0x61, 0x63, 0x72, 0xff, 0x66, 0x65, 0x76, 0x20, 0x54, 0xeb, 0x6c, 0x69, 0x69,
            0x6d, 0x69,
        ];
        assert!(String::from_utf8(bytes.clone()).is_err());
        let utf8_version = "Sacrÿfev Tëliimi".to_string();
        let converted = convert_to_utf8(bytes.clone());
        assert_eq!(converted, utf8_version);
    }

    #[test]
    fn windows1252_is_decoded() {
        // windows-1252 is identical to iso-8859-1. IDEK.
        let bytes: Vec<u8> = vec![
            0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd,
            0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb,
            0xdc, 0xdd, 0xde, 0xdf, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9,
            0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
            0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
        ];
        let utf8_version =
            "ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿ".to_string();
        assert!(String::from_utf8(bytes.clone()).is_err());
        let converted = convert_to_utf8(bytes.clone());
        assert_eq!(converted.len(), utf8_version.len());
        assert_eq!(converted, utf8_version);
    }
}
