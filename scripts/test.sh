cd test/unittest_with_minio/ && \
docker-compose up lhs3util-testlhs3util && \
if [[ $? -eq 0 ]];
then
    echo "TEST SUCCESSFUL"
else
    echo "TEST FAILED[$?]"
fi
