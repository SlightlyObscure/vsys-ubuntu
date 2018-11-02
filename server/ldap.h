#include <ldap.h>

//TODO common include file (gute idee von michi)

#define LDAP_URI "ldap://ldap.technikum-wien.at:389"
#define SEARCHBASE "dc=technikum-wien,dc=at"
#define SCOPE LDAP_SCOPE_SUBTREE
#define FILTER "(uid=if17b*)"
#define BIND_USER ""	/* anonymous bind with user and pw empty */
#define BIND_PW ""

class myLDAP
{
	
//man kann sich in static klaassen schwieriger member sachen merken
// static kann von außerhalb aufgerufen, ohne dass die klasse instanziert werden muss

public:
static string login(string user, string password)
{
   LDAP *ld;			/* LDAP resource handle */
   LDAPMessage *result, *e;	/* LDAP result handle */
   BerElement *ber;		/* array of attributes */
   char *attribute;
   BerValue **vals;

   BerValue *servercredp;
   BerValue cred;
   cred.bv_val = (char *)password.c_str();
   //cred.bv_len=strlen(password);
   cred.bv_len=password.length();
   int i,rc=0;

   const char *attribs[] = { "uid", "cn", NULL };		/* attribute array for search */

   int ldapversion = LDAP_VERSION3;

   /* setup LDAP connection */
   if (ldap_initialize(&ld,LDAP_URI) != LDAP_SUCCESS)
   {
      fprintf(stderr,"ldap_init failed");
      return "ERR\n";
   }

   printf("connected to LDAP server %s\n",LDAP_URI);

   if ((rc = ldap_set_option(ld, LDAP_OPT_PROTOCOL_VERSION, &ldapversion)) != LDAP_SUCCESS)
   {
      fprintf(stderr, "ldap_set_option(PROTOCOL_VERSION): %s\n", ldap_err2string(rc));
      ldap_unbind_ext_s(ld, NULL, NULL);
      return "ERR\n";
   }
    /**/
   if ((rc = ldap_start_tls_s(ld, NULL, NULL)) != LDAP_SUCCESS)
   {
      fprintf(stderr, "ldap_start_tls_s(): %s\n", ldap_err2string(rc));
      ldap_unbind_ext_s(ld, NULL, NULL);
      return "ERR\n";
   }



    user = "uid=" + user + ", ou=people, dc=technikum-wien, dc=at";
   /* non-anonymous bind */
   rc = ldap_sasl_bind_s(ld,user.c_str(),LDAP_SASL_SIMPLE,&cred,NULL,NULL,&servercredp);

   if (rc != LDAP_SUCCESS)
   {
      fprintf(stderr,"LDAP bind error: %s\n",ldap_err2string(rc));
      ldap_unbind_ext_s(ld, NULL, NULL);
      return "ERR\n";
   }
   
    printf("bind successful\n");
    return "OK\n";

}
};
