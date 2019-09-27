// svgcleaner could help you to clean up your SVG files
// from unnecessary data.
// Copyright (C) 2012-2018 Evgeniy Reizner
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

use svgdom::Document;

const NUM_SIZE: usize = 5;
const CHARS_SIZE: usize = 62;
const MAX_NUM_CHAR: u8 = (CHARS_SIZE - 1) as u8;

struct Num {
    d: [u8; NUM_SIZE],
    chars: [u8; CHARS_SIZE],
}

impl Num {
    fn new() -> Num {
        let mut chars = [0u8; CHARS_SIZE];
        {
            let mut pos = 0;
            for c in 97..123 {
                // a-z
                chars[pos] = c;
                pos += 1;
            }
            for c in 65..91 {
                // A-Z
                chars[pos] = c;
                pos += 1;
            }
            for c in 48..58 {
                // 0-9
                chars[pos] = c;
                pos += 1;
            }
        }

        let mut arr = [255u8; NUM_SIZE];
        arr[NUM_SIZE - 1] = 0;

        Num {
            d: arr,
            chars: chars,
        }
    }

    fn shift(&mut self, pos: usize, head: &usize) {
        let n = self.d[pos];

        // Id cannot start with digit, so first value should never overcome 51.
        if n == MAX_NUM_CHAR || (n == 51 && &pos == head) {
            self.d[pos] = 0;

            if self.d[pos - 1] == 255 {
                self.d[pos - 1] = 0;
            } else {
                self.shift(pos - 1, head);
            }
        } else {
            self.d[pos] += 1;
        }
    }

    // Can panic if number is bigger than around 100000000.
    fn plus_one(&mut self) {
        let mut head = NUM_SIZE - 1;
        for i in (0..5).rev() {
            if self.d[i] == 255 {
                head = i + 1;
                break;
            }
        }

        self.shift(NUM_SIZE - 1, &head);
    }

    fn to_string(&self) -> String {
        let mut new = [0u8; NUM_SIZE];
        let mut len = NUM_SIZE;
        for i in (0..NUM_SIZE).rev() {
            if self.d[i] == 255 {
                break;
            }
            len -= 1;

            let n = self.d[i] as usize;
            new[i] = self.chars[n];
        }

        String::from_utf8_lossy(&new[len..NUM_SIZE]).into_owned()
    }
}

pub fn trim_ids(doc: &Document) {
    let mut num = Num::new();
    for (_, mut node) in doc.descendants().svg() {
        if node.has_id() {
            node.set_id(num.to_string());
            num.plus_one();
        }
    }
}

#[cfg(test)]
mod tests {
    use super::Num;

    #[test]
    fn test_1() {
        let mut num = Num::new();
        assert_eq!(num.to_string(), "a");

        num.plus_one();
        assert_eq!(num.to_string(), "b");

        for _ in 0..50 {
            num.plus_one();
        }
        assert_eq!(num.to_string(), "Z");

        // id cannot start with digit,
        // so after 'Z' we'll get 'aa' and not '0'.
        num.plus_one();
        assert_eq!(num.to_string(), "aa");

        for _ in 0..62 {
            num.plus_one();
        }
        assert_eq!(num.to_string(), "ba");

        for _ in 0..62 {
            num.plus_one();
        }
        assert_eq!(num.to_string(), "ca");

        for _ in 0..62 {
            for _ in 0..52 {
                num.plus_one();
            }
        }
        assert_eq!(num.to_string(), "aca");
    }
}
