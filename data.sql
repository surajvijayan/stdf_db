select TEST_NAME,COND_GROUP_ID,COND,count(id) as count  from  ptest_tdigest_cond_view where STDF_ID=1477 and COND_GROUP_ID=199 and COND='VDDO_V1P8:1.674V' group by TEST_NAME,COND_GROUP_ID;


select TEST_NAME,COND_GROUP_ID,COND,count(id) as count  from  ptest_tdigest_cond_view where STDF_ID=1477 and COND_GROUP_ID=199 and COND='VDDO_V1P8:1.674V';


