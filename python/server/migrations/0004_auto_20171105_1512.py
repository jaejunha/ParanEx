# -*- coding: utf-8 -*-
# Generated by Django 1.11.3 on 2017-11-05 06:12
from __future__ import unicode_literals

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('server', '0003_lecture'),
    ]

    operations = [
        migrations.AlterField(
            model_name='lecture',
            name='label',
            field=models.IntegerField(null=True),
        ),
        migrations.AlterField(
            model_name='lecture',
            name='major',
            field=models.CharField(max_length=40, null=True),
        ),
        migrations.AlterField(
            model_name='lecture',
            name='type',
            field=models.CharField(max_length=10, null=True),
        ),
    ]
