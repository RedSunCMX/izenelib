		//////////////////////////////////////////////////////////////////////////
		// ʹ�����ַ�ʽ��ԭ���ǣ����� ar ��Ϊһ�����ò�������ʱ��
		// ���������ֹһЩ�Ż�������� ar �������Ĵ�����
		// �������Ż����µĽ��������ܾܺ޴��ѷ��ֵ����� VC2008 �У��ﵽ 2 ������
		//
		//  expanded as below, 'ar' will be put in register
		//  ���ֹ�չ�� test_serialize, ���������ܽ� ar �Ž��Ĵ������ٶȻ���һ��(d.d4 ��һ��)
		//	test_serialize(ar, d);
		//------------------------------------------------------------------------
		int64_t c0, c1, c2, c3, c4, c5, c6, c7;

		QueryPerformanceCounter(&c0);
		ar & v0;
		QueryPerformanceCounter(&c1);
		ar & v1;
		QueryPerformanceCounter(&c2);
		ar & v2;
		QueryPerformanceCounter(&c3);
		ar & v3;
		QueryPerformanceCounter(&c4);

		for (int i=0, n=v0.size(); i < n; ++i)
		{
			MyData1 md1;
			ar & md1;
		}
		QueryPerformanceCounter(&c5);

		for (int i=0, n=v0.size(); i < n; ++i)
		{
			VarIntD vid;
			ar & vid;
		}
		QueryPerformanceCounter(&c6);

		ar & v4;
		QueryPerformanceCounter(&c7);

		d.d0 += c1 - c0;
		d.d1 += c2 - c1;
		d.d2 += c3 - c2;
		d.d3 += c4 - c3;
		d.d4 += c5 - c4;
		d.d5 += c6 - c5;
		d.d6 += c7 - c6;
		//////////////////////////////////////////////////////////////////////////
