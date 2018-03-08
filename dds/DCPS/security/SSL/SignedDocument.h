/*
 * Distributed under the DDS License.
 * See: http://www.DDS.org/license.html
 */

#ifndef OPENDDS_SECURITY_SSL_SignedDocument_H
#define OPENDDS_SECURITY_SSL_SignedDocument_H

#include "dds/DCPS/security/DdsSecurity_Export.h"
#include "dds/DCPS/unique_ptr.h"
#include "Certificate.h"
#include <string>
#include <openssl/pkcs7.h>

namespace OpenDDS {
  namespace Security {
    namespace SSL {

      class DdsSecurity_Export SignedDocument
      {
      public:

        typedef DCPS::unique_ptr<SignedDocument> unique_ptr;

        SignedDocument(const std::string& uri);

        SignedDocument();

        ~SignedDocument();

        SignedDocument& operator=(const SignedDocument& rhs);

        void load(const std::string& uri);

        int get_content(std::string& dst);

        int verify_signature(const Certificate& cert);

      private:

        PKCS7* PKCS7_from_SMIME(const std::string& path);

        PKCS7* doc_;
        BIO* content_;
        std::string plaintext_;
      };

    }
  }
}

#endif
